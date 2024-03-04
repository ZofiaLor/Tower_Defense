export module User;
import <string>;
import <fstream>;
import <filesystem>;

export class User {
	std::string username;
	int progress;
	bool file_updated, logged_in;

public:

	User() : file_updated(true), logged_in(false) {}

	~User() {
		logOut();
	}

	void logIn(std::string un) {
		username = un;
		std::ifstream ifile("./text/users/" + un + ".txt");
		if (ifile) {
			std::string ts;
			std::getline(ifile, ts);
			progress = std::stoi(ts);
		}
		ifile.close();
		logged_in = true;
	}

	void signIn(std::string un) {
		username = un;
		progress = 1;
		logged_in = true;
		file_updated = false; // nawet jesli od razu wyjdziemy to trzeba utworzyc plik
	}

	void logOut() {
		if (!file_updated && logged_in) {
			std::ofstream ofile("./text/users/" + username + ".txt");
			if (ofile) {
				ofile << progress << "\n";
			}
			ofile.close();
		}
		file_updated = true;
		logged_in = false;
	}

	void logOutDeleteUser() {

		if (std::filesystem::exists("./text/users/" + username + ".txt")) {
			std::filesystem::remove("./text/users/" + username + ".txt");
		}

		file_updated = true;
		logged_in = false;

	}

	void nextLevel() {
		progress++;
		file_updated = false;
	}

	void startOver() {
		progress = 1;
		file_updated = false;
	}


	int getProgress() {
		return progress;
	}

	std::string getUsername() {
		return username;
	}
};