#include "iostream"
#include "string"
#include "vector"
#include "cctype"
#include <sstream>
#include <algorithm>
#include <iterator>
#include "GameManager.h"

using namespace std;

//Enumerations to keep track of the various data that makes up the game world
enum en_DIRS {NORTH, WEST, EAST, SOUTH};
enum en_ROOMS {RED, BLUE, YELLOW, ORANGE, GREEN, PURPLE, PINK, WHITE, BLACK, WINROOM};
enum en_VERBS { TAKE, USE, OPEN, EXAMINE, INVENTORY, LOOK, JUMP, TALK };
enum en_NOUNS { RUBICKSCUBE, PUZZLEROBOT, ROBOCANDY, BOX, LIGHTBULB, BUZZSAW, KEY, DOOR };
enum en_SYNONYMS { RCUBE, CUBE, ROBOT, CANDY, LIGHT, SAW };

//Some useful constants, the lengths of the enums and a "null" value
const int NONE = -1;
const int DIRS = 4;
const int ROOMS = 10;
const int VERBS = 8;
const int NOUNS = 8;
const int SYNONYMS = 6;

//Basic struct for words a player could enter; for example: a direction, a verb
struct word 
{
	string wrd;
	int code;
};

//Struct keeping track of the game objects - nouns that the player interacts with
struct noun
{
	string wrd;						//Name of object
	string description;				//Description of object
	string statusMessage;			//Message indicating what the status bool means
	string unlockMessage;			//Message that prints when player flips the status of this object
	string alternateDescription;	//Description that can be used after player flips status of this object
	int code;						//Numerical code of specific noun
	int content = NONE;				//Any other object that is contained within this one
	int location;					//The location of this object on the map
	bool canCarry;					//Can player carry object?
	bool status;					//Status boolean indicating what interactions are available
	int key = NONE;					//Indicates another object as this one's "key" to unlock
};

//Struct for the rooms that make up the map
struct room 
{
	string roomTitle;
	string description;
	int exits[DIRS];
};

//Instantiate the list of rooms
void create_rooms(room *rms)
{
	rms[RED].roomTitle = "Red Room";
	rms[RED].description = "The walls are painted dark red. It smells like blood..";
	rms[RED].exits[NORTH] = NONE;
	rms[RED].exits[WEST] = NONE;
	rms[RED].exits[EAST] = BLUE;
	rms[RED].exits[SOUTH] = ORANGE;

	rms[BLUE].roomTitle = "Blue Room";
	rms[BLUE].description = "The walls are painted calming blue. The wallpaper is blueberry patterned.";
	rms[BLUE].exits[NORTH] = WHITE;
	rms[BLUE].exits[WEST] = RED;
	rms[BLUE].exits[EAST] = NONE;
	rms[BLUE].exits[SOUTH] = NONE;

	rms[YELLOW].roomTitle = "Yellow Room";
	rms[YELLOW].description = "The walls are painted bright yellow. It's bright in here.";
	rms[YELLOW].exits[NORTH] = NONE;
	rms[YELLOW].exits[WEST] = NONE;
	rms[YELLOW].exits[EAST] = NONE;
	rms[YELLOW].exits[SOUTH] = PURPLE;

	rms[ORANGE].roomTitle = "Orange Room";
	rms[ORANGE].description = "The walls are painted musty orange. The air tastes of orange Febreze and crap. Reminds you of your father.";
	rms[ORANGE].exits[NORTH] = RED;
	rms[ORANGE].exits[WEST] = NONE;
	rms[ORANGE].exits[EAST] = GREEN;
	rms[ORANGE].exits[SOUTH] = PINK;

	rms[GREEN].roomTitle = "Green Room";
	rms[GREEN].description = "The walls are painted lime green. It feels nauseating to stand in here.";
	rms[GREEN].exits[NORTH] = NONE;
	rms[GREEN].exits[WEST] = ORANGE;
	rms[GREEN].exits[EAST] = PURPLE;
	rms[GREEN].exits[SOUTH] = NONE;

	rms[PURPLE].roomTitle = "Purple Room";
	rms[PURPLE].description = "The walls are painted majestic purple. This one actually smells nice, like violets or Barney.";
	rms[PURPLE].exits[NORTH] = YELLOW;
	rms[PURPLE].exits[WEST] = GREEN;
	rms[PURPLE].exits[EAST] = NONE;
	rms[PURPLE].exits[SOUTH] = BLACK;

	rms[PINK].roomTitle = "Pink Room";
	rms[PINK].description = "The walls are painted cotton candy pink. The opening theme from Lazy Town won't stop playing.";
	rms[PINK].exits[NORTH] = ORANGE;
	rms[PINK].exits[WEST] = NONE;
	rms[PINK].exits[EAST] = NONE;
	rms[PINK].exits[SOUTH] = NONE;

	rms[WHITE].roomTitle = "White Room";
	rms[WHITE].description = "The walls are painted bright white. You blend in.";
	rms[WHITE].exits[NORTH] = NONE;
	rms[WHITE].exits[WEST] = NONE;
	rms[WHITE].exits[EAST] = BLACK;
	rms[WHITE].exits[SOUTH] = BLUE;

	rms[BLACK].roomTitle = "Black Room";
	rms[BLACK].description = "The walls are painted deep black. You feel as though you have reached ego death and are floating formless in a vast and infinite... where am I again?";
	rms[BLACK].exits[NORTH] = PURPLE;
	rms[BLACK].exits[WEST] = WHITE;
	rms[BLACK].exits[EAST] = NONE;
	rms[BLACK].exits[SOUTH] = NONE;

	rms[WINROOM].roomTitle = "Win room";
	rms[WINROOM].description = "You finally made it. Out of whatever that tiny set of rooms was. Into a greater nothingness. You are finally free. Go forth, my child. Go forth and create your destiny.\n\n\n ****YOU WIN!!!****";
	rms[WINROOM].exits[NORTH] = NONE;
	rms[WINROOM].exits[WEST] = NONE;
	rms[WINROOM].exits[EAST] = ORANGE;
	rms[WINROOM].exits[SOUTH] = NONE;
}

//Instantiate directions the player can go
void assign_dirs(word *dir)
{
	dir[NORTH].code = NORTH;
	dir[NORTH].wrd = "NORTH";
	dir[WEST].code = WEST;
	dir[WEST].wrd = "WEST";
	dir[EAST].code = EAST;
	dir[EAST].wrd = "EAST";
	dir[SOUTH].code = SOUTH;
	dir[SOUTH].wrd = "SOUTH";
}

//Instantiate verbs the player can use
void assign_verbs(word *vrb)
{
	vrb[TAKE].code = TAKE;
	vrb[TAKE].wrd = "TAKE";
	vrb[USE].code = USE;
	vrb[USE].wrd = "USE";
	vrb[OPEN].code = OPEN;
	vrb[OPEN].wrd = "OPEN";
	vrb[EXAMINE].code = EXAMINE;
	vrb[EXAMINE].wrd = "EXAMINE";
	vrb[INVENTORY].code = INVENTORY;
	vrb[INVENTORY].wrd = "INVENTORY";
	vrb[LOOK].code = LOOK;
	vrb[LOOK].wrd = "LOOK";
	vrb[JUMP].code = JUMP;
	vrb[JUMP].wrd = "JUMP";
	vrb[TALK].code = TALK;
	vrb[TALK].wrd = "TALK";
}

//Instantiate all the game objects as nouns with various indicators and data fields
void assign_nouns(noun *nns)
{
	nns[RUBICKSCUBE].wrd = "RUBICKSCUBE";
	nns[RUBICKSCUBE].description = "A Rubick's cube(tm), twisted and distorted beyond human comprehension.";
	nns[RUBICKSCUBE].alternateDescription = "A Rubick's cube(tm), neatly reorganized by mechanical processing.";
	nns[RUBICKSCUBE].statusMessage = "Solved: ";
	nns[RUBICKSCUBE].unlockMessage = "The robot grabs the cube from your hands and after a dizzying " 
		"split second blur of color, \nhands you back the solved Rubick's Cube. He could've at least made it look a little challenging..";
	nns[RUBICKSCUBE].code = RUBICKSCUBE;
	nns[RUBICKSCUBE].location = WHITE;
	nns[RUBICKSCUBE].canCarry = true;
	nns[RUBICKSCUBE].status = false;
	nns[RUBICKSCUBE].key = PUZZLEROBOT;

	nns[PUZZLEROBOT].wrd = "PUZZLEROBOT";
	nns[PUZZLEROBOT].description = "Some sort of puzzle-solving robot. He looks distracted.";
	nns[PUZZLEROBOT].alternateDescription = "Your friendly neighborhood puzzle-bot. Said he owed you a favor.";
	nns[PUZZLEROBOT].statusMessage = "Willing to help: ";
	nns[PUZZLEROBOT].code = PUZZLEROBOT;
	nns[PUZZLEROBOT].location = PINK;
	nns[PUZZLEROBOT].canCarry = false;
	nns[PUZZLEROBOT].status = false;
	nns[PUZZLEROBOT].key = LIGHTBULB;

	nns[ROBOCANDY].wrd = "ROBOCANDY";
	nns[ROBOCANDY].description = "It looks like.. metal.. candy? Some kind of.. Robocandy?";
	nns[ROBOCANDY].statusMessage = "HIDDEN";
	nns[ROBOCANDY].code = ROBOCANDY;
	nns[ROBOCANDY].location = RED;
	nns[ROBOCANDY].canCarry = false;
	nns[ROBOCANDY].status = false;

	nns[BOX].wrd = "BOX";
	nns[BOX].description = "Just a small metal box with a biiig lock.";
	nns[BOX].alternateDescription = "Just an unlocked small metal box without its biiig lock.";
	nns[BOX].statusMessage = "LOCKED";
	nns[BOX].unlockMessage = "The lock opens with a satisfying click, and lands on the "
		"ground with a satisfying thud. What a satisfactory lock!";
	nns[BOX].code = BOX;
	nns[BOX].content = ROBOCANDY;
	nns[BOX].location = RED;
	nns[BOX].canCarry = false;
	nns[BOX].status = true;
	nns[BOX].key = KEY;

	nns[LIGHTBULB].wrd = "LIGHTBULB";
	nns[LIGHTBULB].description = "A lightbulb, tightly mounted onto the floor. Wonder how to get it out?";
	nns[LIGHTBULB].alternateDescription = "A loose lightbulb, the fruit of your labor.";
	nns[LIGHTBULB].statusMessage = "Loose: ";
	nns[LIGHTBULB].unlockMessage = "It may seem like overkill but in desparate times, overkill can be \na desparate measure."
		" You slice the lightbulb free, but it takes about an hour with you being so scared to break the glass.";
	nns[LIGHTBULB].code = LIGHTBULB;
	nns[LIGHTBULB].content = KEY;
	nns[LIGHTBULB].location = YELLOW;
	nns[LIGHTBULB].canCarry = false;
	nns[LIGHTBULB].status = false;
	nns[LIGHTBULB].key = BUZZSAW;

	nns[BUZZSAW].wrd = "BUZZSAW";
	nns[BUZZSAW].description = "A pretty scary looking buzzsaw.";
	nns[BUZZSAW].statusMessage = "Ready to rip: ";
	nns[BUZZSAW].code = BUZZSAW;
	nns[BUZZSAW].location = BLACK;
	nns[BUZZSAW].canCarry = true;
	nns[BUZZSAW].status = true;

	nns[KEY].wrd = "KEY";
	nns[KEY].description = "A tiny key.";
	nns[KEY].statusMessage = "HIDDEN";
	nns[KEY].code = KEY;
	nns[KEY].location = YELLOW;
	nns[KEY].canCarry = false;
	nns[KEY].status = false;

	nns[DOOR].wrd = "DOOR";
	nns[DOOR].description = "A huge, colorful, ornate door to the West with a shape in the middle that looks just like a Rubick's Cube!";
	nns[DOOR].alternateDescription = "A wide open doorway West to a new world of possibilities! Another dimension awaits!";
	nns[DOOR].statusMessage = "LOCKED";
	nns[DOOR].unlockMessage = "Like a majestic palace gate made entirely of legos, the door accepts your \nplaything-offering and slowly opens,"
	" revealing what literally looks like that scene \nfrom the Matrix where Neo is learning new techniques."
	"With all the white space for miles and miles? \nToo bad you can't make a sandwich appear or learn Kung Fu in this reality.";
	nns[DOOR].code = DOOR;
	nns[DOOR].location = ORANGE;
	nns[DOOR].canCarry = false;
	nns[DOOR].status = true;
	nns[DOOR].key = RUBICKSCUBE;

}

//Instantiate the synonyms for various nouns
void assign_synonyms(word *syn)
{
	syn[RCUBE].code = RUBICKSCUBE;
	syn[RCUBE].wrd = "RUBICK'SCUBE";
	syn[CUBE].code = RUBICKSCUBE;
	syn[CUBE].wrd = "CUBE";
	syn[ROBOT].code = PUZZLEROBOT;
	syn[ROBOT].wrd = "ROBOT";
	syn[CANDY].code = ROBOCANDY;
	syn[CANDY].wrd = "CANDY";
	syn[LIGHT].code = LIGHTBULB;
	syn[LIGHT].wrd = "LIGHT";
	syn[SAW].code = BUZZSAW;
	syn[SAW].wrd = "SAW";
}

void parse_raw(string cmd, string &w1, string &w2)
{
	istringstream iss(cmd);
	vector<string> words{ istream_iterator<string>{iss},
		istream_iterator<string>{} };
	if (words.size() > 3)
	{
		cout << "I can only read the first few words of your commands. After that I get bored." << endl;
		return;
	}
	else if (words.size() == 0)
	{
		cout << "What's that? Speak up!!" << endl;
		return;
	}
	else {
		unsigned int i = 0;
		for (i = 0; i < words[0].size(); i++)
			words[0].at(i) = toupper(words[0].at(i));
		w1 = words[0];
		if (words.size() >= 2)
		{
			for (i = 0; i < words[1].size(); i++)
				words[1].at(i) = toupper(words[1].at(i));
			w2 = words[1];
		}
		if (words.size() == 3)
		{
			for (i = 0; i < words[2].size(); i++)
				words[2].at(i) = toupper(words[2].at(i));
			w2 += words[2];
		}
	}
}

//Function to display all the verbs to the player, as well as some tips
void display_help(word *vrbs)
{
	for (int i = 0; i < VERBS; i++)
	{
		cout << vrbs[i].wrd << "\n";
	}
	cout << "You can also type any compass direction to move in that direction.\n\n" <<
		"To use an object, first enter \"Use (Object)\", THEN, when prompted, type what you would like to use it on.\n\n" <<
		"All other actions follow the form \"(verb) (noun)\" and will not work with any extra words.\n" << endl
		<< "Type \"QUIT\" at any time to wuss out." << endl;
}

//Function to print out information about the room the player is in
void look_around(int loc, word *dirs, room *rms, noun *nns)
{
	cout << rms[loc].roomTitle << "\n" << rms[loc].description  << "\n" << endl;
	for (int i = 0; i < DIRS; i++) {
		if (rms[loc].exits[i] != NONE)
			cout << "You see the " << rms[rms[loc].exits[i]].roomTitle << " to the " << dirs[i].wrd << "." << endl;
	}
	cout << endl;
	string result = "";
	for (int i = 0; i < NOUNS; i++) {
		if (nns[i].location == loc && nns[i].statusMessage != "HIDDEN")
			result += nns[i].description + "\n";
	}
	if (!result.empty())
		cout << "In this room: \n" << result;
}

//Function allowing the player to take an object in the same location as them
bool take_object(int loc, noun& n)
{
	if (n.statusMessage != "HIDDEN" && n.canCarry && (loc == n.location))
	{
		n.location = NONE;
		cout << "Taken." << endl;
		return true;
	}
	else if (!n.canCarry && loc == n.location)
	{
		cout << "Your embarassingly stringy muscles tear under \nthe effort. Guess you'll have to find another way." << endl;
		return false;
	}	
	else
	{
		cout << "You're looking for what ain't there." << endl;
		return false;
	}
}

//Function allowing the player to observe an object in the same room or in their possession
void examine_object(int loc, noun& n)
{
	if (n.statusMessage != "HIDDEN" && (loc == n.location || n.location == NONE))
	{
		cout << n.description << endl;
		if (n.statusMessage == "LOCKED" && n.status)
			cout << "Locked." << endl;
		else if (n.statusMessage == "LOCKED" && !n.status)
			cout << "Unlocked." << endl;
		else cout << n.statusMessage << ((n.status) ? "yes" : "no") << endl;
	}
	else
	{
		cout << "You try to examine what isn't there and get lost in a thought loop." << endl;
	}
}

//Function that lists the player's inventory
void check_inventory(noun *nns)
{
	cout << "Your inventory: " << endl;
	for (int i = 0; i < NOUNS; i++)
	{
		if (nns[i].location == NONE)
			cout << nns[i].wrd << endl;
	}
}

//Function that attempts to open a container object and reveals what it holds
void open_object(int loc, noun n, noun *nns)
{
	if (n.statusMessage == "LOCKED" && !n.status) {
		nns[n.content].statusMessage = "Shiny: ";
		nns[n.content].status = true;
		nns[n.content].canCarry = true;
		cout << "Opened, revealing: " << nns[n.content].description << endl;
	}
	else cout << "You can't open it! No surprise there." << endl;
}

//Function that "unlocks" a locked object, switching its status to allow the player to interact with it more
//Also makes certain objects carryable, prints out description of unlocking the object, and switches the object description
void unlock_object(noun& n)
{
	n.status = !n.status;
	if(n.statusMessage != "LOCKED")
		n.canCarry = true;
	if (!n.unlockMessage.empty())
		cout << endl << n.unlockMessage << endl;
	if (!n.alternateDescription.empty())
	{
		n.description = n.alternateDescription;
	}
}

//Find a specific noun's numerical code given a text entry to search for it by name
int find_noun(string &w, noun *nns, word *syns)
{
	for (int i = 0; i < NOUNS; i++)
	{
		if (w == nns[i].wrd)
			return nns[i].code;
	}
	for (int i = 0; i < SYNONYMS; i++)
	{
		if (w == syns[i].wrd)
			return syns[i].code;
	}
	return NONE;
}

//Endgame condition: opens up hidden room, allows player access, player wins when they enter and game ends
void endgame(room *rms) 
{
	for (int i = 0; i < ROOMS; i++)
	{
		if (rms[i].roomTitle == "Orange Room")
			rms[i].exits[WEST] = WINROOM;
	}
}

//Allows player to use an item on an object in their location
void use_object(int loc, noun& n, noun *nns, word *syns, room *rms)
{
	if (n.location == NONE) 
	{
		cout << "What do you want to use the " << n.wrd << " on?" << endl;
		string temp;
		string temp1;
		string temp2;
		getline(cin, temp);
		parse_raw(temp, temp1, temp2);			//Get input same way as main
		temp1 += temp2;
		noun &subject = nns[find_noun(temp1, nns, syns)];
		if (subject.key == n.code && n.status)				//Check if the "key" on the other object is the item player has, and item is activated
		{
			if (subject.code == PUZZLEROBOT) {		//Robot has different behaviors for object interaction, he requires two "keys"
				if (n.code == LIGHTBULB) {
					subject.key = ROBOCANDY;
					cout << "The robot spins its head 360 degrees and extends a claw to grasp the bulb. " <<
						"\n\n\"BZZT thank you! I needed the illumination. " <<
						"Now if only I had a snack to think through this puzzle..\"\n" << endl;
				}
				else {
					unlock_object(subject);
					cout << "As if it could sense you approaching, the robot started blinking and whistling excitedly. " <<
						"\n\n\"BBZZZTT A MUCH NEEDED SNACK! Thank you strange humanoid! I will grant you one favor.\"" << endl;
				}
			}
			else if (subject.code == DOOR)	
			{
				unlock_object(subject);
				endgame(rms);
			}
			else
			{
				unlock_object(subject);
			}
			n.location = -2; //"Discards" object after its purpose is served
				
		}
		else if (subject.code == n.key && subject.status)
		{
			unlock_object(n);
		}
		else 
		{
			cout << "You mash the " + n.wrd + " against the " + subject.wrd + " to no avail. Although, it made a cool sound!" << endl;
		}
	}
	else
	{
		cout << "You don't have one of those. Maybe if you wish hard enough?" << endl;
	}
}

//Allows players to talk to characters (Currently only robot)
void talk_to(int loc, noun n)
{
	if (loc == n.location && n.statusMessage != "HIDDEN")
	{

		if (n.code == PUZZLEROBOT && !n.status)
		{
			switch (n.key) {				//Robot's state controls what he says
				case LIGHTBULB: cout << "Before you can even finish a word, the robot makes a loud, grinding, " <<
					"buzzing noise and stares directly at you.\n\n\"SILENCE SMALL-MINDED FLESH-BEING; I NEED FOCUS FOR CALCULATING THIS PUZZLE. " <<
					"However, visibility is relatively low currently. I wish I had a new lightbulb so my sensors could tell me what I am doing..\"\n"; break;
				case ROBOCANDY: cout << "This time the robot seems a little more pleased (whatever that means) with your presence.\n" <<
					"\n\"Greetings soft companion, this unit expresses appreciation for the improvement \nof lighting. However, my hunger sensors" <<
					" are now high and interfering \nwith important crosswording. Please keep an eye out for food units.\"\n"; break;
			}
		}
		else
		{							//Default random collection of phrases when player tries to talk to other objects
			string chatArray[4] = { "You mumble very quietly at the , but it doesn't respond.",
				"You babble incoherently at the . Weird.", "You scream profanities at the  . It secretly hates you." };
			int RandIndex = rand() % 3;
			cout << chatArray[RandIndex].insert(31, n.wrd) << endl;
		}
	}
	else
	{
		cout << "You wish to call into the void, but your puny vocal chords tremble and crack." << endl;
	}
}

//After parsing the raw input from the player, parse their verb/noun syntax and call appropriate methods
bool parse_command(int &loc, string w1, string w2, word *dir, word *vrbs, noun *nns, word *syns, room *rms) 
{
	int i;
	int VERB_CODE = NONE;
	int NOUN_CODE = NONE;

	if (w1 == "HELP")
	{
		display_help(vrbs);
		return false;
	}
	if (w1 == "QUIT" || (w1 == "" && w2 == ""))
	{
		return false;
	}

	for (i = 0; i < DIRS; i++)				//Check word 1 against directions
	{
		if (w1 == dir[i].wrd)
		{
			if (rms[loc].exits[dir[i].code] != NONE)
			{
				loc = rms[loc].exits[dir[i].code];
				look_around(loc, dir, rms, nns);
				return true;
			}
			else
			{
				cout << "You bump into a wall, OUCH!" << endl;
				return true;
			}
		}
	}
	for (i = 0; i < VERBS; i++)				//Check word 1 against verbs
	{
		if (w1 == vrbs[i].wrd)
		{
			VERB_CODE = vrbs[i].code;
			break;
		}
	}
	if (VERB_CODE == NONE)				//Break out of bogus verb
	{
		cout << "Try again." << endl;
		return false;						//Method's return type is used to track if player is struggling, to tell them about the Help command, as well as advance the timer
	}
	if (w2 != "")
	{
		NOUN_CODE = find_noun(w2, nns, syns);				//Check word 2 against nouns

		if (NOUN_CODE == NONE)						//Break out of bogus noun
		{
			cout << "You don't see any " << w2 << "." << endl;
			return false;
		}

		if (VERB_CODE == TAKE)					//Take object: calls take_object method, reveals any contents that the object was obscuring
		{
			bool taken = take_object(loc, nns[NOUN_CODE]);
			if (nns[NOUN_CODE].content != NONE && taken)
			{
				nns[nns[NOUN_CODE].content].statusMessage = "Shiny: ";
				nns[nns[NOUN_CODE].content].status = true;
				nns[nns[NOUN_CODE].content].canCarry = true;
				cout << "Taking that item revealed the following: " << nns[nns[NOUN_CODE].content].description;
			}
			return true;
		}
		if (VERB_CODE == EXAMINE)		//Attempt to examine a specific noun in a location, or in player possession
		{
			examine_object(loc, nns[NOUN_CODE]);
			return true;
		}

		if (VERB_CODE == OPEN)			//Attempt to open an object in the location
		{
			open_object(loc, nns[NOUN_CODE], nns);
			return true;
		}
		
		if (VERB_CODE == USE)			//Attempt to use a given item from the player's inventory on another object
		{
			use_object(loc, nns[NOUN_CODE], nns, syns, rms);
			return true;
		}

		if (VERB_CODE == TALK)			//Attempt to talk to a game object
		{
			talk_to(loc, nns[NOUN_CODE]);
			return true;
		}

	}
	else if (w2 == "" && (VERB_CODE == TAKE || VERB_CODE == USE ||
		VERB_CODE == OPEN || VERB_CODE == EXAMINE || VERB_CODE == TALK))		//Catch partial input, ask for completing noun
	{
		cout << "What would you like to perform this action on?" << endl;
		string temp;
		string temp1;
		getline(cin, temp);
		parse_raw(temp, w2, temp1);
		w2 += temp1;
		return parse_command(loc, w1, w2, dir, vrbs, nns, syns, rms);
	}

	if (VERB_CODE == LOOK)				//Call method to get information on current location
	{
		look_around(loc, dir, rms, nns);
		return true;
	}

	if (VERB_CODE == JUMP)				//Jump, just for fun
	{
		cout << "You jump in place, wasting precious time, but having\n a blast in the process. Was it really a waste?" << endl;
		return true;
	}

	if (VERB_CODE == INVENTORY)			//Call method to display character inventory
	{
		check_inventory(nns);
		return true;
	}

	
	return false;						//If we didn't catch anything else, return false
}

//Instantiate game objects, rooms, words, player, etc., maintain game loop
int main()
{
	room rooms[ROOMS];
	create_rooms(rooms);
	word dirs[DIRS];
	assign_dirs(dirs);
	word verbs[VERBS];
	assign_verbs(verbs);
	noun nouns[NOUNS];
	assign_nouns(nouns);
	word synonyms[SYNONYMS];
	assign_synonyms(synonyms);

	int playerLocation = GREEN;

	string command;
	string word1;
	string word2;
	int timer = 50;
	int stuckTimer = 3;

	cout << "Type \"QUIT\" at any time to wuss out.\n" << endl;
	cout << "You wake up in a mysteriously colorful series of rooms, \nwith no order or sanity governing what is within them.. \n" <<
		"Do you have what it takes to find your way out of this vaguely surreal prison?" << endl << endl;
	cout << rooms[playerLocation].roomTitle << "\n" << rooms[playerLocation].description << endl;
	cout << "\nYou have " << timer << " moves left until the world implodes.\n" << endl;

	while (word1 != "QUIT" && timer > 0)			//Game loop: timer ticks every move, quit command also leaves game
	{
		cout << "What's next? ";
		command.clear();
		getline(cin, command);						//Prompt for input, parse it, send it to command parser
		cout << endl;
		word1.clear();
		word2.clear();
		parse_raw(command, word1, word2);
		if (parse_command(playerLocation, word1, word2, dirs, verbs, nouns, synonyms, rooms))		//On successful command, tick timer
		{
			timer--;
			cout << "\nYou have " << timer << " moves left until the world implodes.\n" << endl;
		}	
		else
		{
			stuckTimer--;
			if (stuckTimer == 0)					//If player is struggling with commands, tell them about Help
			{
				cout << "Type \"Help\" for a list of viable commands." << endl;
				stuckTimer = 3;
			}
		}
		if (playerLocation == WINROOM)										//Win case
		{
			system("pause");
			return 0;
		}
			
	}
	cout << "The world has imploded!!! KABOOOSH YOU LOSE" << endl;			//Lose case
	system("pause");
	return 0;
}