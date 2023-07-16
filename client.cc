#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_number>" << std::endl;
        return 1;
    }

    // Extract the file number from command-line argument
    int fileNumber = std::stoi(argv[1]);

    // Map the file number to the corresponding filename
    std::string filename = "file" + std::to_string(fileNumber) + ".txt";

    // Create socket
    int senderSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (senderSocket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set up server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888); // Receiver's port
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported." << std::endl;
        close(senderSocket);
        return 1;
    }

    // Connect to the server
    if (connect(senderSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Connection failed." << std::endl;
        close(senderSocket);
        return 1;
    }

    // Read file
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file." << std::endl;
        close(senderSocket);
        return 1;
    }

    // Send file size
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (send(senderSocket, reinterpret_cast<char *>(&fileSize), sizeof(fileSize), 0) == -1)
    {
        std::cerr << "Failed to send file size." << std::endl;
        close(senderSocket);
        file.close();
        return 1;
    }

    // Send file data
    char buffer[BUFFER_SIZE];
    while (!file.eof())
    {
        file.read(buffer, BUFFER_SIZE);
        if (send(senderSocket, buffer, file.gcount(), 0) == -1)
        {
            std::cerr << "Failed to send file data." << std::endl;
            close(senderSocket);
            file.close();
            return 1;
        }
    }

    // Close connection
    close(senderSocket);
    file.close();

    std::cout << "File sent successfully." << std::endl;

    return 0;
}
