#include <iostream>
#include <filesystem>
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>


//for knowing what order is the printing
enum orderEnum
{
    ORDER_DIRS_FIRST = 1,
    ORDER_FILES_FIRST
};

//shortcut for std::filesystem
namespace fs = std::filesystem;

//struct to keep track of an entry and its size
struct EntryInfo
{
    fs::directory_entry entry;
    std::uintmax_t size;
};


constexpr int INDEX_COMMAND_ARGV = 1;
constexpr int ONE_COMMAND = 2;
constexpr int SPACES_PER_LEVEL = 4;

//consts for formating files sizes
constexpr std::uintmax_t BYTES_PER_KB = 1024;
constexpr std::uintmax_t BYTES_PER_MB = BYTES_PER_KB * 1024;
constexpr std::uintmax_t BYTES_PER_GB = BYTES_PER_MB * 1024;
constexpr std::uintmax_t BYTES_PER_TB = BYTES_PER_GB * 1024;

//the commands
const std::string COMMAND_DIR_FIRST = "--dirs-first";
const std::string COMMAND_FILE_FIRST = "--files-first";
const std::string COMMAND_ONLY_FILES = "--files";
const std::string COMMAND_ONLY_DIRS = "--dirs";
const std::string COMMAND_NO_RECURSION = "--no-recursive";
const std::string COMMAND_SORT_NAME = "--sort-name";
const std::string COMMAND_SORT_SIZE = "--sort-size";
const std::string COMMAND_HELP = "--help";

bool compareByName(const fs::directory_entry& a, const fs::directory_entry& b);
bool compareBySize(const EntryInfo& a, const EntryInfo& b);
void printTotalSize(std::uintmax_t totalSize);
void printHelp();

std::string formatFileSize(std::uintmax_t fileSize);

std::uintmax_t printDirsFirst(const fs::path& path, int depth);
std::uintmax_t printDirs(const fs::path& path, int depth, int order);
std::uintmax_t printFiles(const fs::path& path, int depth);

std::uintmax_t printOnlyFiles(const fs::path& path);
void printOnlyDirs(const fs::path& path, int depth);

std::uintmax_t printNoRecursive(const fs::path& path);
std::uintmax_t printFilesFirst(const fs::path& path, int depth);
std::uintmax_t printSortedName(const fs::path& path, int depth);
std::uintmax_t getDirectorySize(const fs::path& path);
std::uintmax_t printSortedSize(const fs::path& path, int depth);

int main(int argc, char* argv[])
{

    std::uintmax_t totalSize = 0;

    if (argc != ONE_COMMAND)
    {
        std::cout << "Error: only one command after ./miniVault" << std::endl;
    }

    else if (argv[INDEX_COMMAND_ARGV] == COMMAND_DIR_FIRST)
    {
        totalSize = printDirsFirst(".", 0);
        printTotalSize(totalSize);
    }

    else if (argv[INDEX_COMMAND_ARGV] == COMMAND_ONLY_FILES)
    {
        totalSize = printOnlyFiles(".");
        printTotalSize(totalSize);
    }

    else if (argv[INDEX_COMMAND_ARGV] == COMMAND_ONLY_DIRS)
    {
        printOnlyDirs(".", 0);
    }

    else if (argv[INDEX_COMMAND_ARGV] == COMMAND_NO_RECURSION)
    {
        totalSize = printNoRecursive(".");
        printTotalSize(totalSize);
    }

    else if (argv[INDEX_COMMAND_ARGV] == COMMAND_FILE_FIRST)
    {
        totalSize = printFilesFirst(".", 0);
        printTotalSize(totalSize);
    }

    else if(argv[INDEX_COMMAND_ARGV] == COMMAND_SORT_NAME)
    {
        totalSize = printSortedName(".", 0);
        printTotalSize(totalSize);
    }

    else if(argv[INDEX_COMMAND_ARGV] == COMMAND_SORT_SIZE)
    {
        totalSize = printSortedSize(".", 0);
        printTotalSize(totalSize);
    }

    else if(argv[INDEX_COMMAND_ARGV] == COMMAND_HELP)
    {
        printHelp();
    }

    else
    {
        std::cout << "Error: invalid command\nenter ./miniVault --help to know more about the available commands" << std::endl;
    }


    return 0;
}

//printing the desc about the commands
void printHelp()
{
    std::cout << "miniVault - filesystem explorer\n";
    std::cout << "\n";
    std::cout << "Usage: ./miniVault [command]\n";
    std::cout << "\n";
    std::cout << "Commands:\n";
    std::cout << "  --help           Show this help message\n";
    std::cout << "  --dirs-first     Show directories before files\n";
    std::cout << "  --files-first    Show files before directories\n";
    std::cout << "  --files          Show only files\n";
    std::cout << "  --dirs           Show only directories\n";
    std::cout << "  --no-recursive   Show only the current directory\n";
    std::cout << "  --sort-name      Sort entries by name\n";
    std::cout << "  --sort-size      Sort entries by size\n";
}

//comparing the sizes between two files
bool compareBySize(const EntryInfo& a, const EntryInfo& b)
{
    return a.size < b.size;
}

//print the files but sorted by size from smallest to biggest
std::uintmax_t printSortedSize(const fs::path& path, int depth)
{
    std::uintmax_t totalSize = 0;
    std::vector<EntryInfo> entries;
    EntryInfo entry;

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            if (item.is_directory())
            {
                entry.size = getDirectorySize(item.path());
                entry.entry = item;
            }

            else
            {
                entry.size = item.file_size();
                entry.entry = item;
            }

            entries.push_back(entry);
        }

        std::sort(entries.begin(), entries.end(), compareBySize);

        for (const EntryInfo& currEntry : entries)
        {
            if (depth != 0)
            {
                std::cout << std::string(SPACES_PER_LEVEL * depth, ' ');
                std::cout << "└──";
            }

            if (currEntry.entry.is_directory())
            {
                std::cout << "[DIR]\t" << currEntry.entry.path().filename() << std::endl;

                totalSize += printSortedSize(currEntry.entry.path(), depth + 1);
            }

            else
            {
                std::cout << "[FILE]\t" << currEntry.entry.path().filename() << " " << formatFileSize(currEntry.size) << std::endl;

                totalSize += currEntry.size;
            }
        }


    }

    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;
    }
    

    return totalSize;
}

//getting the dir size
std::uintmax_t getDirectorySize(const fs::path& path)
{
    std::uintmax_t totalSize = 0;
    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            if (item.is_directory())
            {
                totalSize += getDirectorySize(item.path());
            }
            else
            {
                totalSize += item.file_size();
            }
        }
    }

    catch(const std::exception& e)
    {
        return totalSize;
    }

    return totalSize;
}

//print the total size of the dir
void printTotalSize(std::uintmax_t totalSize)
{
    std::cout << "\nThe total size is: " << formatFileSize(totalSize) << std::endl;
}

//comparing names of two files
bool compareByName(const fs::directory_entry& a, const fs::directory_entry& b)
{
    return a.path().filename() < b.path().filename();
}

//print files but sorted by name
std::uintmax_t printSortedName(const fs::path& path, int depth)
{
    std::vector<fs::directory_entry> entries;
    std::uintmax_t totalSize = 0;

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            entries.push_back(item);
        }   

        std::sort(entries.begin(), entries.end(), compareByName);


        for (const fs::directory_entry& entry : entries)
        {
            if (depth != 0)
            {
                std::cout << std::string(SPACES_PER_LEVEL * depth, ' ');
                std::cout << "└──";
            }
            if (entry.is_directory())
            {
                std::cout << "[DIR]\t" << entry.path().filename() << std::endl;
                totalSize += printSortedName(entry.path(), depth + 1);
            }
            else
            {
                std::cout << "[FILE]\t" << entry.path().filename() << " " << formatFileSize(entry.file_size()) << std::endl;

                totalSize += entry.file_size();
            }
        }

    }
    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;;
    }
    
    return totalSize;

}

//print the files first then the dirs
std::uintmax_t printFilesFirst(const fs::path& path, int depth)
{
    std::uintmax_t totalSize = 0;

    totalSize = printFiles(path, depth);
    totalSize += printDirs(path, depth, ORDER_FILES_FIRST);

    return totalSize;

}

//print the entries but without recursion
std::uintmax_t printNoRecursive(const fs::path& path)
{
    std::uintmax_t totalSize = 0;

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            if (item.is_directory())
            {
                std::cout << "[DIR]\t" << item.path().filename() << std::endl;
            }
            else
            {
                std::cout << "[FILE]\t" << item.path().filename() << " " << formatFileSize(item.file_size()) << std::endl;

                totalSize += item.file_size();
            }
        }
    }

    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;
    }

    return totalSize;
    
}

//print only the dirs
void printOnlyDirs(const fs::path& path, int depth)
{

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            if (item.is_directory())
            {  
                std::cout << std::string(depth * SPACES_PER_LEVEL, ' ');

                if (depth != 0)
                {
                    std::cout << "└──";
                }  

                std::cout << "[DIR]\t" << item.path().filename() << std::endl;
                printOnlyDirs(item.path(), depth + 1);
            }
  


        }
    }

    
    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;
    }
    
}

//print only the files
std::uintmax_t printOnlyFiles(const fs::path& path)
{
    std::uintmax_t totalSize = 0;

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            if (item.is_directory())
            {
                totalSize += printOnlyFiles(item.path());
            }

            else
            {
                std::cout << "[FILE]\t" << item.path().filename() << " " << formatFileSize(item.file_size()) << std::endl;

                totalSize += item.file_size();
            }
        }
    }

    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;
    }
    
    return totalSize;
}

//print normal files
std::uintmax_t printFiles(const fs::path& path, int depth)
{
    std::uintmax_t totalSize = 0;

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {
            if (!item.is_directory())
            {
                std::cout << std::string(depth * SPACES_PER_LEVEL, ' ');
                if (depth != 0)
                {
                    std::cout << "└──";
                }
                std::cout << "[FILE]\t" << item.path().filename() << " " << formatFileSize(item.file_size()) << std::endl;
                
                totalSize += item.file_size();
            }
        }    
    }

    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;
    }
    
    return totalSize;
}

//print normal dirs
std::uintmax_t printDirs(const fs::path& path, int depth, int order)
{

    std::uintmax_t totalSize = 0;

    try
    {
        for (const auto& item : fs::directory_iterator(path))
        {



            if (item.is_directory())
            {            
                std::cout << std::string(depth * SPACES_PER_LEVEL, ' ');

                if (depth != 0)
                {
                    std::cout << "└──";
                }
                std::cout << "[DIR]\t" << item.path().filename() << std::endl;

                if (order == ORDER_DIRS_FIRST)
                {
                    totalSize += printDirsFirst(item.path(), depth + 1);
                }
                else
                {
                    totalSize += printFilesFirst(item.path(), depth + 1);
                }
            }

            
        }
    }

    catch(const std::exception& e)
    {
        std::cout << "[DENIED]\t" << path << std::endl;
    }

    return totalSize;
    
}

//print the dirs first
std::uintmax_t printDirsFirst(const fs::path& path, int depth)
{
    std::uintmax_t totalSize = 0;

    totalSize = printDirs(path, depth, ORDER_DIRS_FIRST);
    totalSize += printFiles(path, depth);

    return totalSize;
}

//formating the size from bytes to tb/gb/mb/kb
std::string formatFileSize(std::uintmax_t fileSize)
{
    std::ostringstream output;
    std::string unit = "";
    float convertedFileSize = 0;

    if (fileSize >= BYTES_PER_TB)
    {
        convertedFileSize = (float)fileSize / BYTES_PER_TB;
        unit = " TB";
    }
    else if (fileSize >= BYTES_PER_GB)
    {
        convertedFileSize = (float)fileSize / BYTES_PER_GB;
        unit = " GB";
    }
    else if (fileSize >= BYTES_PER_MB)
    {
        convertedFileSize = (float)fileSize / BYTES_PER_MB;
        unit = " MB";
    }
    else if (fileSize >= BYTES_PER_KB)
    {
        convertedFileSize = (float)fileSize / BYTES_PER_KB;
        unit = " KB";
    }
    else
    {
        unit = " B";
        return std::to_string(fileSize) + unit;

    }


    output << std::fixed << std::setprecision(1) << convertedFileSize;

    return output.str() + unit;

}