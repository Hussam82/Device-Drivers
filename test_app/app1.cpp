#include <fstream>
#include <iostream>

class Driver
{
private:
    const std::string PATH = "/dev/test_file";
    std::fstream m_fd;

public:
    void WriteFile(std::string message);
    std::string ReadFile();
};

class Display
{
private:
    /* data */
    static bool flag;

public:
    static void DisplayOnChange(std::string value)
    {
        if(value.find("1") != std::string::npos && flag == false)
        {
            std::cout << "LED is on now" << std::endl;
            flag = true;
        }
        else if (value.find("0") != std::string::npos && flag==true)
        {
            /* code */
            std::cout << "LED is off now" << std::endl;
            flag = false;
        }
    }
};

bool Display::flag = false;

void Driver::WriteFile(std::string message)
{
    m_fd.open(PATH, std::ios::out);
    m_fd.write(message.c_str(), message.size());
    m_fd.close();
}

std::string Driver::ReadFile()
{
    std::string result;
    m_fd.open(PATH);
    std::getline(m_fd, result);
    m_fd.close();
    return result;
}

int main()
{
    std::string input;
    Driver d;
    std::cout << "The application is Running ... " << std::endl;
    while (1)
    {
        std::string input = d.ReadFile();
        d.WriteFile(input);
        Display::DisplayOnChange(input);
    }
    return 0;
}