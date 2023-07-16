#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void receiveFile(int senderSocket, const std::string &filename)
{
    // Receive file size
    int fileSize;
    if (recv(senderSocket, reinterpret_cast<char *>(&fileSize), sizeof(fileSize), 0) <= 0)
    {
        std::cerr << "Failed to receive file size for " << filename << std::endl;
        return;
    }

    // Receive file data
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to create file " << filename << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;
    int totalBytesRead = 0;

    while (totalBytesRead < fileSize)
    {
        bytesRead = recv(senderSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
        {
            std::cerr << "Failed to receive file data for " << filename << std::endl;
            return;
        }

        file.write(buffer, bytesRead);
        totalBytesRead += bytesRead;
    }

    file.close();

    std::cout << "Received file: " << filename << std::endl;
}

int main()
{
    // Create socket
    int receiverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (receiverSocket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set up server address
    struct sockaddr_in serverAddress, clientAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888); // Receiver's port
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the specified address and port
    if (bind(receiverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Bind failed." << std::endl;
        return 1;
    }

    // Listen for incoming connections
    listen(receiverSocket, 3); // Allow up to 3 connections

    std::cout << "Waiting for the senders to connect..." << std::endl;

    std::vector<std::thread> threads;

    for (int i = 0; i < 3; ++i)
    {
        // Accept a connection from a sender
        socklen_t clientAddressSize = sizeof(clientAddress);
        int senderSocket = accept(receiverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (senderSocket < 0)
        {
            std::cerr << "Accept failed." << std::endl;
            return 1;
        }

        std::string filename = "file" + std::to_string(i + 1) + "recv.txt";
        threads.emplace_back(receiveFile, senderSocket, filename);
    }

    // Wait for all threads to finish
    for (std::thread &thread : threads)
    {
        thread.join();
    }

    // Close connection
    // close(senderSocket);
    // close(receiverSocket);

    std::cout << "All files received successfully." << std::endl;

    return 0;
}
