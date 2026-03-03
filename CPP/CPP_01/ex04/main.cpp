/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 14:38:36 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/16 17:48:15 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

int main(int ac, char **av)
{
	if (ac != 4)
	{
		std::cerr << "Need: " << av[0] << "<filename> <replaced_str> <replacement_str>" << std::endl;
		return 1;
	}

	std::string file = av[1];
    std::string s1 = av[2];
    std::string s2 = av[3];
	if (s1.empty())
	{
		std::cerr << "Error:  s1 must not be empty" <<std::endl;
		return 1;
	}

	std::ifstream ifs(file.c_str());
	if (!ifs)
	{
		std::cerr << "Error: could not open file " << file << std::endl;
		return 1;
	}

	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	std::string result;
	std::size_t i = 0;
	std::size_t found;
	while((found = content.find(s1, i)) != std::string::npos)
	{
		result.append(content, i, found - i);
		result.append(s2);
		i = found + s1.length();
	}
	result.append(content, i, content.size() - i);

	std::string newfile = file + ".replace";
	std::ofstream ofs(newfile.c_str());
	if (!ofs)
	{
		std::cerr << "Error: could not create file " << newfile << std::endl;
		return 1;
	}
	ofs << result;
	ofs.close();
	
	return 0;
}