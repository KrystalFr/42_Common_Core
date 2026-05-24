/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:22 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/24 14:40:25 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>

void process_input(const std::string input, const BitcoinExchange &btc)
{
	std::ifstream infile(input.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}

	std::string line;
	while(getline(infile, line))
	{
		if (line.empty())
			continue;

		
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
			btc = new BitcoinExchange("data.cvs");
		}
		catch (const std::exception &)
		{
			btc = new BitcoinExchange("../data.cvs");
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		delete btc;
		return 1;
	}

	processInput(input, *btc);
	delete btc;
	return 0;
}