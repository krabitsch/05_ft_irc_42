
#include <string>
#include <map>

class Client
{
	private:
	int fd; //Fd Socket
	std::string nickname; //Nickname
	std::string hostname; //resolved hsotname or the IP of the user
	std::string username; //Default username set by the system for explain aruckenb or pvass //The username is bascially the home user
	std::string password; //User password

	std::map<std::string, char> channels; //Which channels the user is apart of and the char is either a m or o for member or operator
	std::string current_channel; //Should this exist or not is there a better way
	std::map<char, std::string> notifications; //Map containing invite to channel or private msg, as well as a char to indicaite what type of notification it is
	
	public:
	Client(int fd, std::string username);
	~Client();
	Client(const Client &type);
	Client &operator=(const Client &type1);

	//Client Info
	std::string GetNickname(void);
	void SetNickname(std::string newname);
	int GetFdSocket(void);
	std::string GetUsername(void);

	//Client Channel Features
	std::map<std::string, char> *GetChannel(void);
	void SetChannel(std::map<std::string, char> *newchannels);
	std::string GetCurrentChannel(void);
	void SetCurrentChannel(std::string newchannel);
	void AddChannel(std::string channelname);

	//Notifications *Still unsure about this part honestly
	void addNofitication(std::string msg, char type);
};