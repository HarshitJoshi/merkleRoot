#include <cstdlib> 
#include <cstring>
#include <iostream> 
#include <cassert>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <openssl/sha.h>
#include <jsoncpp/json/json.h>

using namespace std;

string sha256(const string str);
string print_hash_as_hex(const string& hash);
string hex_to_ascii(const string &hex);
string reverse(string& src);
string hasher(string &a, string &b);
vector<string> merkle(vector <string> &txHashes);

// debug
int ROUND = 1;
bool print = false;

int main() {

    CURL *curl;
    FILE *fp;
    CURLcode res;
    
    string url;
    cout << "Enter the URL: "; // API URL that contains the block information
    getline(cin, url);
    
    char outfilename[FILENAME_MAX] = "block.json";
    curl = curl_easy_init();

    if (curl) {
        fp = fopen(outfilename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    ifstream ifs("block.json");
    
    if (ifs.is_open()) {
        cout << "File success!" << endl;
    } else {
        cout << "Error: Unable to open file." << endl;
    }

    Json::Reader reader;
    Json::Value root;
    
    bool parsingSuccessful = reader.parse(ifs, root);
    
    if (!parsingSuccessful) {
        cout << "Error: Unable to process the file." << reader.getFormattedErrorMessages();
    } else {
        cout << "Parsing successful." << endl;
    }

    cout << "Block #" << root["height"] << endl;
    
    bool isValid = !root["tx"].isNull();
    cout << "Has Transactions (1 or 0): " << isValid << endl;
    cout << "Total number of transactions in the block: " << root["tx"].size() << endl;
    cout << "Merkle root of the block: " << root["merkleroot"].asString() << endl;
    
    vector <string> txHashes;
    
    for (int i = 0; i < root["tx"].size(); i++) {

        txHashes.push_back(root["tx"][i].asString());
    }

    vector <string> rootHash = merkle(txHashes);
    
    if (print) {
        cout << "Merkle vector: " << rootHash.at(0) << endl;
        cout << "Merkle root from block-info: " << root["merkleroot"].asString() << endl << endl;
    }

    if (rootHash.at(0).compare(root["merkleroot"].asString()) != 0) {
        cout << "Merkle root is not equal." << endl;
    } else {
        cout << "Merkle root is verified." << endl;
    }
    
    return 0;
}

string sha256(const string str) {
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    
    return string((char*) hash, 32); // returns the binary digest in string
}

string print_hash_as_hex(const string &hash) {
    
    stringstream ss;

    if (hash.length() == 32) {
        for (size_t i = 0; i < 32; i++) {
            ss << hex << setw(2) << setfill('0') << (int)(unsigned char)hash[i];
        }
    }
    
   return ss.str(); // return the copy of contents in the stream buffer
}

string hex_to_ascii(const string &hex) {

    int length = hex.length();
    string newString;

    for (int i = 0; i < length; i += 2) { 
        string byte = hex.substr(i, 2);
        char chr = (char)(int) strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    
    return newString;
}

string hasher(string &first, string &second) {

    /* Reverse inputs before and after hashing
     * https://github.com/bitcoin-dot-org/bitcoin.org/issues/580 */
    
    string firstReverse = hex_to_ascii(first);
    string secondReverse = hex_to_ascii(second);
    
    firstReverse = reverse(firstReverse);
    secondReverse = reverse(secondReverse);
    
    string concat = firstReverse + secondReverse;
    
    string SHA = sha256(sha256(concat)); // take double SHA256 hash of concatenated string
    string reverse_SHA = reverse(SHA);
    
    return print_hash_as_hex(reverse_SHA);
}

string reverse(string& src) {
    
    for (int head = 0, tail = src.length() - 1; head < tail; head++, tail--) {
        char ch = src[head];
        src[head] = src[tail];
        src[tail] = ch;
    }
    
    return src;
}

vector<string> merkle(vector <string> &txHashes) {
    ROUND = ROUND + 1; // debug variable
    
    if (txHashes.size() == 1) {
        cout << "CALCULATED MERKLE ROOT IS: ";
        cout << txHashes[0] << endl;
        return txHashes;
    }

    vector <string> temp;
    
    /* go through the whole txHashes vector alternating by every two, grab them
     * and hash those pairs together and push them in the temp vector skipping the last*/
    for (int i = 0; i < txHashes.size() - 1; i = i + 2) {
        // debug
        if (print) {
            cout << "\nBranch " << i + 1 << " is " << txHashes[i] << endl;
            cout << "Branch " << i + 2 << " is " << txHashes[i + 1] << endl;
            cout << "\ntheir hash is " << hasher(txHashes[i], txHashes[i + 1]) << endl;
        }

        temp.push_back(hasher(txHashes[i], txHashes[i + 1]));
    }
    
    /* if the size of txHashes vector is odd then the last element without the pair
     is hashed with itself and the output is pushed in the temp vector*/
    if (txHashes.size() % 2 == 1) {
        //debug
        if (print) {
            cout << "\nBranch " << txHashes.size() << " is " << txHashes[txHashes.size() - 1];
            cout << "\nand Branch " << txHashes.size() << " is hashed with itself to get " <<
                    hasher(txHashes[txHashes.size() - 1], txHashes[txHashes.size() - 1]);
        }

        temp.push_back(hasher(txHashes[txHashes.size() - 1], txHashes[txHashes.size() - 1]));
    }
    
    //debug
    if (print) {
        cout << "\n\nDONE with Round" << ROUND << endl;
        cout << "<========================================================>";
        cout << endl << endl << endl;
    }

    return merkle(temp);
}