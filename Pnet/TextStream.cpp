#include "TextStream.h"

void TextStream::LoadUsers(std::string textfileName)
{
    std::string line;
    std::ifstream myfile(textfileName + ".bin");
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            size_t found = line.find("~/`/~3#");
            if (found != std::string::npos)
            {
                std::cout << "\t\t\t\t\t" << line.substr(7, line.length()) << std::endl << std::endl;
            }
            else
            {
                std::cout << line << std::endl;
                std::cout << ">>>" << std::endl << std::endl;
            }
        }
        myfile.close();
    }
     else std::cout << "FAILED TO LOAD FILE";

}

bool TextStream::userExit(const std::string file)
{
    return false;
}


void TextStream::WriteToText(const std::string textfileName, const std::string str)
{   
    std::ofstream myfile(textfileName+".bin",std::ios::app);
    
    if (myfile.is_open())
    {
        myfile << str;
        myfile.close();
    }
    else
        std::cout << "unable to open file:: data.bin" << std::endl;
}



void TextStream::ReadText(std::string& file)
{
    std::string line;
    std::ifstream myfile(file,std::ios::in||std::ios::binary);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            const char pos[3] = { line[line.size() - 3], line[line.size() - 2], line[line.size() - 1] };
                if(pos == "<<<")
                {
                    std::cout << line << std::endl;
                }
                else std::cout << "\n\n \t\t\t\t\t" << line << "\n\n" << std::endl;
           
        }
        myfile.close();
    }
    else
        std::cout << "Unable to open file";
}
