
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

	public:
	Client();
	~Client();
	Client(const Client &type);
	Client &operator=(const Client &type1);


	std::string GetNickname(void);
	void SetNickname(std::string newname);
};