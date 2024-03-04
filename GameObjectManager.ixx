export module GameObjectManager;
import <vector>;
import <map>;
import <string>;
import <utility>;
import <algorithm>;
import <filesystem>;
import <fstream>;
import <regex>;
import <SFML/Graphics.hpp>;
import Turret;
import Projectile;
import Enemy;
import GameObject;
import Tile;


export class GameObjectManager {

	// Kolizje: Turret - Enemy, Enemy - Projectile
	static const enum ObjectType {turret, projectile, enemy};
	std::vector<std::pair<ObjectType, GameObject*>> objects;
	struct tcl {
		sf::Clock clock;
		int type;
		int index;
		int cooldown;
		GameObject* turret_pointer;
	};
	// Zegar danej wiezyczki, typ, indeks na siatce, przerwa miedzy kolejnymi pociskami, wskaznik na wiezyczke
	std::vector<tcl> turret_clocks;
	int enemies_in_row[5];
	

	std::map<std::string, sf::Texture> textures;
	std::map<int, std::vector<std::string>> enemy_types;
	std::map<int, std::vector<std::string>> turret_types;
	std::map<int, std::vector<std::string>> projectile_types;

	void makeTypeDescs(std::map<int, std::vector<std::string>>& descs, std::string path) {
		std::regex reg("[0-9]+|[0-9]+\\.[0-9]+|[a-zA-Z]+_[0-9]+");

		std::ifstream ifile(path);
		if (ifile) {
			std::vector<std::string> v;
			std::string ts;
			while (std::getline(ifile, ts)) {
				if (ts == "") {
					descs[std::stoi(v[0])] = v;
					v.clear();
				}

				else if (std::regex_match(ts, reg)) {
					v.push_back(ts);
				}

			}
			if (v.size() != 0)
				descs[std::stoi(v[0])] = v;
		}
		ifile.close();
	}

public:

	GameObjectManager() {

		for (int i = 0; i < 5; i++)
			enemies_in_row[i] = 0;
		
		for (auto& itr : std::filesystem::directory_iterator("./images/goms")) {
			std::string s = itr.path().stem().string();
			sf::Texture t;
			t.loadFromFile(itr.path().string());
			textures[s] = t;
		}

		makeTypeDescs(enemy_types, "./text/type_descs/enemy_types.txt");
		makeTypeDescs(turret_types, "./text/type_descs/turret_types.txt");
		makeTypeDescs(projectile_types, "./text/type_descs/projectile_types.txt");


	}

	~GameObjectManager() {
		for (int i = 0; i < objects.size(); i++) {
			delete objects[i].second;
		}
	}

	

	void addTurret(int type, int index) {
		//Turret(int i, int type, int h, sf::Texture& t)
		objects.push_back(std::make_pair(turret, new Turret(index, type, std::stoi(turret_types[type][2]), textures[turret_types[type][1]])));
		
		
		sf::Clock c;
		tcl t = {c, type, index, std::stoi(turret_types[type][3]), objects[objects.size() - 1].second };
		turret_clocks.push_back(t);
	}

	void addProjectile(int type, int index) {
		//Projectile(int i, int type, int dmg, double s, int e, sf::Texture& t)
		objects.push_back(std::make_pair(projectile, new Projectile(index, type, std::stoi(projectile_types[type][2]), std::stod(projectile_types[type][3]), std::stoi(projectile_types[type][4]), textures[projectile_types[type][1]])));
		
	}

	void addEnemy(int type, int index) {
		//Enemy(int i, int type, int h, int dmg, int drop, double s, sf::Texture& t)
		objects.push_back(std::make_pair(enemy, new Enemy(index, type, std::stoi(enemy_types[type][2]), std::stoi(enemy_types[type][3]), std::stod(enemy_types[type][4]), std::stoi(enemy_types[type][5]), textures[enemy_types[type][1]])));
		
		enemies_in_row[index / 8] += 1;
	}

	void spawnProjectiles() {
		for (int i = 0; i < turret_clocks.size(); i++) {
			if (enemies_in_row[turret_clocks[i].index / 8] != 0 && turret_clocks[i].clock.getElapsedTime().asSeconds() >= turret_clocks[i].cooldown) {
				addProjectile(turret_clocks[i].type, turret_clocks[i].index);
				turret_clocks[i].clock.restart();
			}
		}
	}

	void drawAll(sf::RenderWindow& window) {
		for (const auto& object : objects) {
			object.second->Draw(window);
		}
	}

	bool updateAll(const float& dt, int& money, int& enemies_on_map, std::vector<Tile*>& tiles) {

		std::vector<sf::FloatRect> turrets;
		std::vector<std::tuple<sf::FloatRect, int, int>> projectiles;
		std::vector<std::pair<sf::FloatRect, int>> enemies;

		for (const auto& object : objects) {
			if (object.first == turret) {
				turrets.push_back(object.second->getBounds());
			}
			else if (object.first == projectile) {
				projectiles.push_back(std::make_tuple(object.second->getBounds(), object.second->getDamage(), object.second->getEffect()));
			}
			else {
				enemies.push_back(std::make_pair(object.second->getBounds(), object.second->getDamage()));
			}
		}

		int it = 0;
		while (it < objects.size()) {
			if (objects[it].second->update(dt, turrets, projectiles, enemies)) {
				it++;
			}
			else {
				if (objects[it].first == turret) {
					for (int i = 0; i < turret_clocks.size(); i++) {
						if (turret_clocks[i].turret_pointer == objects[it].second) {
							turret_clocks.erase(turret_clocks.begin() + i);
							break;
						}
					}
					tiles[objects[it].second->getIndex()]->setTurretPlaced(false);
				}
				else if (objects[it].first == enemy) {
					if (objects[it].second->getBounds().left <= 0.0) {
						return false;
					}
					enemies_in_row[objects[it].second->getRow()] -= 1;
					enemies_on_map--;
					money += objects[it].second->getDrop();
				}

				delete objects[it].second;
				objects.erase(objects.begin() + it);
			}
		}

		spawnProjectiles();
		return true;
	}

	void clearAll(std::vector<Tile*>& tiles) {
		for (int i = 0; i < objects.size(); i++) {
			delete objects[i].second;
		}
		objects.clear();
		turret_clocks.clear();
		for (int i = 0; i < 5; i++)
			enemies_in_row[i] = 0;
		for (int i = 0; i < tiles.size(); i++) {
			tiles[i]->setTurretPlaced(false);
		}
	}

};