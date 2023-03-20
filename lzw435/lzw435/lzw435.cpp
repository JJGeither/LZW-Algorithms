#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream fs1(file1, std::ios::binary);
    std::ifstream fs2(file2, std::ios::binary);

    if (!fs1.is_open() || !fs2.is_open()) {
        std::cerr << "Error opening files\n";
        return false;
    }

    char c1, c2;
    while (fs1.get(c1) && fs2.get(c2)) {
        if (c1 != c2) {
            fs1.close(); // Close files before exiting function
            fs2.close();
            return false;
        }
    }

    // Check if both files have reached EOF
    if (fs1.eof() && fs2.eof()) {
        fs1.close(); // Close files before exiting function
        fs2.close();

        // Display contents of file1
        std::cout << "Contents of " << file1 << ":\n";
        std::ifstream fs3(file1);
        std::string line;
        while (std::getline(fs3, line)) {
            std::cout << line << '\n';
        }
        fs3.close();

        // Display contents of file2
        std::cout << "Contents of " << file2 << ":\n";
        std::ifstream fs4(file2);
        while (std::getline(fs4, line)) {
            std::cout << line << '\n';
        }
        fs4.close();

        return true;
    }
    else {
        fs1.close(); // Close files before exiting function
        fs2.close();
        return false;
    }
}

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

std::vector<int> read_numbers(std::string filename) {
    std::ifstream infile(filename);
    std::vector<int> numbers;

    if (!infile) {
        std::cout << "Error opening file" << std::endl;
        return numbers;
    }

    int num;
    while (infile >> num) {
        numbers.push_back(num);
    }

    infile.close();
    return numbers;
}


template<typename T>
void writeToVector(const std::string& filepath, const  std::vector<T>& data)
{

    std::ofstream filestream(filepath);
    std::copy(data.begin(), data.end(), std::ostream_iterator<T>(filestream, " "));
    filestream.close();

}

void writeToString(const std::string& filename, const std::string& contents) {
    // Create an output file stream object and open the file for writing
    std::ofstream outfile(filename, std::ofstream::trunc);

    // Write the contents to the file
    outfile << contents;

    // Close the file
    outfile.close();
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
        return -1;
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
        writeToVector("testFile.lzw", compressed);

    }

    if (operation == "e") {
     
        std::vector<int> fileNumbers =  read_numbers(filename);
        std::string decompressed = decompress(fileNumbers.begin(), fileNumbers.end());
        writeToString("testFile2.lzw", decompressed);

        if (compareFiles("testFile2.lzw", "testFile.txt")) {
            std::cout << "Identical" << std::endl;
        }
        else
            std::cout << "Different" << std::endl;
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
*/