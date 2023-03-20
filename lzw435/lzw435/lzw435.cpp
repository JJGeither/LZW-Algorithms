#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>


std::string readFile(const std::string& fileName) {
    std::string fileContents;

    std::ifstream file(fileName);

    if (file.is_open()) {
        //Read the contents
        std::string line;
        while (getline(file, line)) {
            fileContents += line + '\n';
        }
        file.close();
    }
    else {
        return "Unable to open file";
    }
    return fileContents;
}

template<typename T>
void writeTo(const std::string& filepath, const  std::vector<T>& data)
{

    std::ofstream filestream(filepath);
    std::copy(data.begin(), data.end(), std::ostream_iterator<T>(filestream, " "));
    filestream.close();

}

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const std::string& uncompressed, Iterator result) {
    // Build the dictionary.
    int dictSize = 256;
    std::map<std::string, int> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[std::string(1, i)] = i;

    std::string w;
    for (std::string::const_iterator it = uncompressed.begin();
        it != uncompressed.end(); ++it) {
        char c = *it;
        std::string wc = w + c;
        if (dictionary.count(wc))
            w = wc;
        else {
            *result++ = dictionary[w];
            // Add wc to the dictionary.
            dictionary[wc] = dictSize++;
            w = std::string(1, c);
        }
    }

    // Output the code for w.
    if (!w.empty())
        *result++ = dictionary[w];
    return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
std::string decompress(Iterator begin, Iterator end) {
    // Build the dictionary.
    int dictSize = 256;
    std::map<int, std::string> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[i] = std::string(1, i);

    std::string w(1, *begin++);
    std::string result = w;
    std::string entry;
    for (; begin != end; begin++) {
        int k = *begin;
        if (dictionary.count(k))
            entry = dictionary[k];
        else if (k == dictSize)
            entry = w + w[0];
        else
            throw "Bad compressed k";

        result += entry;

        // Add w+entry[0] to the dictionary.
        dictionary[dictSize++] = w + entry[0];

        w = entry;
    }
    return result;
}



int main(int argc, char* argv[]) {
    std::cout << "OKAY" << std::endl;
    if (argc != 3) {
        std::cout << "Usage: : lzw435 c filename to compress and lzw435 e filename.lzw to expand.";
    }

    std::string operation = argv[1];
    std::string filename = argv[2];

    std::vector<int> compressed;
    if (operation == "c") {
        std::string fileContents = readFile(filename);
        compress(fileContents, std::back_inserter(compressed));

        copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(std::cout, ", "));

        std::cout << "File:";
        std::vector<int> to_vector;
        //std::copy(compressed.begin(), compressed.end(), std::back_inserter(to_vector));
        for (int i = 0; i < compressed.size(); i++)
            std::cout << compressed.at(i) << ' ';
        std::cout << std::endl;
        writeTo("testFile.lzw", compressed);

    }

    if (operation == "e") {
        std::string decompressed = decompress(compressed.begin(), compressed.end());
        std::cout << decompressed << std::endl;
        return 0;
    }


    return 0;
}

/*
Part I: LZW using code length 12
• Download https://www.cs.uakron.edu/~duan/classes/435/projects/project2_LZW/lzw435.cpp or the C++ LZW
directly from http://rosettacode.org/wiki/LZW_compression#C.2B.2B and run the program;
• Understand how LZW works and how it’s implemented;
• Modify the program to clean any unnecessary parts and be able to compress any file you are given;
Once we compile the program, we should be able to run it using: lzw435 c “filename” to compress the file with
filename “filename”. You program should save the compressed file as: “filename.lzw”. And we should be able to run
it using: “lzw435 e filename.lzw” to expand the compressed file “filename.lzw”. You program should save the
expanded file as: “filename2”. You know your filename2 should be identical to “filename”. Use “diff” to double
check.

Part II: Modified LZW to allow the length of LZW codes to increase gradually.
• Your algorithm will increase the length of the code words from 9 to 16 bits.
• Do nothing when the length reaches 16 bits.
Name your modified LZW program “lzw435M.cpp”. Once we compile the program, we should be able to run it using:
“lzw435M.exe c filename” to compress the file “filename”. You program should save the compressed file as:
“filename.lzw2”. And we should be able to run it using: “lzw435M.exe e filename.lzw2” to expand the compressed
file “filename.lzw2”. You program should save the expanded file as: “filename.2M”. You know your file named
“filename.2M” should be identical to that of name “filename”.
*/