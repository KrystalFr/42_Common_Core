/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:22 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/04 02:36:45 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}

	std::string inputFile = av[1];

	std::ifstream inFile(inputFile.c_str());
	if (!inFile.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
	}
	
}