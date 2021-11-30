#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;

std::string name;
bool done = false;
std::vector<std::string> clientHistory;

//refreshes  chat with clint history of messages
void refreshMessages() {
	system("clear");
	for (std::string line : clientHistory) {
		std::cout << line +"\n";
	}
        std::cout << "-----------------------------------\n";
	std::cout << "Message: ";
	std::cout << std::flush;
}

class ReadThread : public Thread
{
private:
        //socket
	Socket *socket;

public:
	//constructor - initalize socket
	ReadThread(Socket* socketInput)
		: socket(socketInput)
	{
	}

	//deconstructor - delete is handled in main function
	~ReadThread(){}

	//main thread method
	virtual long ThreadMain()
	{
                //response variable for socket
		ByteArray response;

                //loop until exited
		while (true)
		{
			socket->Read(response);
			std::string responseString = response.ToString();
                        
                        if(responseString == "done"){
                                done = true;
                                break;
                        }
                        //add responseString to local histroy and refresh
			clientHistory.push_back(responseString);
			refreshMessages();
			
		}
		return 1;
	}
};

int main(void)
{
	// Welcome the user
	std::cout << "SE3313 Chat Room Client" << std::endl;

	// Create our socket
	Socket socket("127.0.0.1", 3000);

	// Initially we need a connection
	while (true)
	{
		try
		{
			std::cout << "Connecting...";
			std::cout.flush();
			socket.Open();
			std::cout << "OK\n" << std::endl;
                        break;
		}
		catch (...)
		{
		}

		// Try again every 5 seconds
		std::cout << "Trying again in 5 seconds" << std::endl;
		sleep(5);
	}

	std::cout<< "Enter Name: ";
	std::getline(std::cin, name);

	//command to get messages/start thred
        //socket.Write();

        //start read thread for client
	ReadThread* readThread = new ReadThread(&socket);
        //input variable
	std::string input;

	//continuely ask for input
	while (!done)
	{
		//get input
		std::getline(std::cin, input);

                if(input == "done"){
                        socket.Write(input);
                        done = true;
                        break;
                } else {
                        //send input to server as user:message
		        socket.Write(name + ": " +input);
                        clientHistory.push_back(name + ": " +input);
			refreshMessages();
                }


		//if user wants to end, send it to server as well, then end client
		if (input == "done") 
		{
			//terimate
			std::cout << "\n Ending client session..." << '\n';
			delete readThread;
			break;
		}
	}

	//close socket
	socket.Close();
	return 0;
}