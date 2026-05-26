/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:22 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/26 16:11:13 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

std::string trim(const std::string &str)
{
	std::string::size_type start = str.find_first_not_of(" \t");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = str.find_last_not_of(" \t");
	return str.substr(start, end - start + 1);
}

bool parse_val(const std::string &str, double &val)
{
	//on lit str et si elle a des caractere stockable dans un double
	// on les met dans val, ex: "42.5" -> 42.5
	std::stringstream ss(str);
	ss >> val;
	if (ss.fail())
		return false;
	//on check si y a un character invalide apres val, ex "42.5x"
	char leftover;
	if (ss >> leftover)
		return false;
	return true;
}

void process_input(const std::string input, const BitcoinExchange &btc)
{
	std::ifstream infile(input.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;
	while(std::getline(infile, line))
	{
		if (line.empty())
			continue;
			
		std::string::size_type sep = line.find('|');
		if (sep == std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
//extrait une sous chaine a line et place l'avant separateur dans date et l'apres dans val_str
		std::string date = trim(line.substr(0, sep));
		std::string val_str = trim(line.substr(sep + 1));

		double val;
		if (!parse_val(val_str, val))
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (val < 0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}
		if (val > 1000)
		{
			std::cerr << "Error: too large a number." << std::endl;
			continue;
		}

		try
		{
			double rate = btc.get_rate(date);
			std::cout << date << " => " << val << " = " << rate * val << std::endl;
		}
		catch (const std::exception &)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
		}
	}
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}
	std::string input = av[1];
	
	BitcoinExchange *btc = NULL;
	try
	{
		try
		{
			btc = new BitcoinExchange("data.csv");
		}
		catch (const std::exception &)
		{
			btc = new BitcoinExchange("../data.csv");
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		delete btc;
		return 1;
	}

	process_input(input, *btc);
	delete btc;
	return 0;
}