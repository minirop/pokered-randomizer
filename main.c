#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "pcg_basic.h"

#define GO(offset) fseek(f, offset, SEEK_SET)
#define SKIP(offset) fseek(f, offset, SEEK_CUR)
#define WHERE() ftell(f)

uint8_t read8(FILE * f);
void write8(FILE * f, uint8_t x);
int isThisMissingNo(uint32_t index);
void writeRandomPokemon(FILE * f);
/*
static const char * const names[] = {
	"", "Rhydon", "Kangaskhan", "Nidoran", "Clefairy", "Spearow", "Voltorb", "Nidoking", "Slowbro", "Ivysaur", "Exeggutor", "Lickitung", "Exeggcute", "Grimer",
	"Gengar", "Nidoran♀", "Nidoqueen", "Cubone", "Rhyhorn", "Lapras", "Arcanine", "Mew", "Gyarados", "Shellder", "Tentacool", "Gastly", "Scyther", "Staryu",
	"Blastoise", "Pinsir", "Tangela", "MissingNo", "MissingNo", "Growlithe", "Onix", "Fearow", "Pidgey", "Slowpoke", "Kadabra", "Graveler", "Chansey","Machoke",
	"Mr. Mime", "Hitmonlee", "Hitmonchan", "Arbok", "Parasect", "Psyduck", "Drowzee", "Golem", "MissingNo", "Magmar", "MissingNo", "Electabuzz", "Magneton",
	"Koffing", "MissingNo", "Mankey", "Seel", "Diglett", "Tauros", "MissingNo", "MissingNo", "MissingNo", "Farfetch", "Venonat", "Dragonite", "MissingNo",
	"MissingNo", "MissingNo", "Doduo", "Poliwag", "Jynx", "Moltres", "Articuno", "Zapdos", "Ditto", "Meowth", "Krabby", "MissingNo", "MissingNo", "MissingNo",
	"Vulpix", "Ninetales", "Pikachu", "Raichu", "MissingNo", "MissingNo", "Dratini", "Dragonair", "Kabuto", "Kabutops", "Horsea", "Seadra", "MissingNo",
	"MissingNo", "Sandshrew", "Sandslash", "Omanyte", "Omastar", "Jigglypuff", "Wigglytuff", "Eevee", "Flareon", "Jolteon", "Vaporeon", "Machop", "Zubat",
	"Ekans", "Paras", "Poliwhirl", "Poliwrath", "Weedle", "Kakuna", "Beedrill", "MissingNo", "Dodrio", "Primeape", "Dugtrio", "Venomoth", "Dewgong", "MissingNo",
	"MissingNo", "Caterpie", "Metapod", "Butterfree", "Machamp", "MissingNo", "Golduck", "Hypno", "Golbat", "Mewtwo", "Snorlax", "Magikarp", "MissingNo",
	"MissingNo", "Muk", "MissingNo", "Kingler", "Cloyster", "MissingNo", "Electrode", "Clefable", "Weezing", "Persian", "Marowak", "MissingNo", "Haunter",
	"Abra", "Alakazam", "Pidgeotto", "Pidgeot", "Starmie", "Bulbasaur", "Venusaur", "Tentacruel", "MissingNo", "Goldeen", "Seaking", "MissingNo", "MissingNo",
	"MissingNo", "MissingNo", "Ponyta", "Rapidash", "Rattata", "Raticate", "Nidorino", "Nidorina", "Geodude", "Porygon", "Aerodactyl", "MissingNo", "Magnemite",
	"MissingNo", "MissingNo", "Charmander", "Squirtle", "Charmeleon", "Wartortle", "Charizard", "MissingNo", "MissingNo", "MissingNo", "MissingNo", "Oddish",
	"Gloom", "Vileplume", "Bellsprout", "Weepinbell", "Victreebel"
};
*/
uint8_t read8(FILE * f)
{
	uint8_t x;
	fread(&x, 1, 1, f);
	return x;
}

void write8(FILE * f, uint8_t x)
{
	fwrite(&x, 1, 1, f);
}

static uint32_t missingno[] = {
	 31,  32,  50,  52,  56,  61,  62,  63,  67,  68,
	 69,  79,  80,  81,  86,  87,  94,  95, 115, 121,
	122, 127, 134, 135, 137, 140, 146, 156, 159, 160,
	161, 162, 172, 174, 175, 181, 182, 183, 184
};

int isThisMissingNo(uint32_t index)
{
	for (int i = 0;i < 39;i++)
	{
		if (index == missingno[i])
		{
			return 1;
		}
	}
	return 0;
}

void writeRandomPokemon(FILE * f)
{
	uint32_t index = 31;
	while (isThisMissingNo(index))
	{
		index = pcg32_boundedrand(190) + 1;
	}
	write8(f, index & 0xFF);
}

int main(int argc, char** argv)
{
	int rounds = 5;
	if (argc != 2)
	{
		puts("error: no rom");
		return 1;
	}

	FILE * f = fopen(argv[1], "r+b");
	if (f == NULL)
	{
		puts("error: can't open");
		return 1;
	}

	pcg32_srandom((uint64_t)(time(NULL) ^ (intptr_t)&printf), (uint64_t)&rounds);

	// first shown pokemon on the title screen
	GO(0x4399);
	writeRandomPokemon(f);

	// all pokemons shown during the title screen
	GO(0x4588);
	for (int i = 0;i < 16;i++)
	{
		writeRandomPokemon(f);
	}

	// pokémon during Chen's intro
	GO(0x616D);
	writeRandomPokemon(f);

	// encounters (59 x 2 data, the first one is "no mons", so only 58*2)
	GO(0xD0DF);
	for (int i = 0;i < 58*2;i++)
	{
		int first = read8(f);
		if (first)
		{
			for (int j = 0;j < 10;j++)
			{
				SKIP(1); // skip the level
				writeRandomPokemon(f);
			}
		}
	}

	// trainers' parties
	GO(0x39D99);
	for (int i = 0;i < 391;i++)
	{
		int level = read8(f);
		if (level == 0xFF)
		{
			// level of the pokemon
			level = read8(f);

			// if "0", no more pokemons
			while (level != 0)
			{
				writeRandomPokemon(f);
				level = read8(f);
			}
		}
		else
		{
			// only pokemons
			int numberOfPokemons = 0;
			long pos = WHERE();

			int pkmn = read8(f);
			// if "0", no more pokemons
			while (pkmn != 0)
			{
				numberOfPokemons++;
				pkmn = read8(f);
			}

			GO(pos);
			for (int j = 0;j < numberOfPokemons;j++)
			{
				writeRandomPokemon(f);
			}
			SKIP(1);
		}
	}

	// trades
	GO(0x71B7B);
	for (int i = 0;i < 10;i++)
	{
		writeRandomPokemon(f);
		writeRandomPokemon(f);
		SKIP(12);
	}

	fclose(f);

	return 0;
}
