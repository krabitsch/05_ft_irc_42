
#include <string>
#include <vector>
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
	std::vector<std::string> notifications; //This array contains all notifications, invites to channels and private messages
	
	public:
	Client();
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

	//Notifications *Still unsure about this part honestly
};