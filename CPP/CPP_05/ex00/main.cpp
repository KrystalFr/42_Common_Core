/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 03:24:42 by krfranco          #+#    #+#             */
/*   Updated: 2025/11/11 15:05:45 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Bureaucrat.hpp"

//Bureaucrat: name, grade
int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "NEED: " << av[0] << " <add/sub> <valid/error>" << std::endl;
		return 1;
	}

	std::string arg = av[1];
	std::string state = av[2];
	if ((arg != "add" && arg != "sub") || (state != "valid" && state != "error"))
	{
		std::cout << "NEED: " << av[0] << " <add/sub> <valid/error>" << std::endl;
		return 1;
	}

	int loop = 149;
	try
	{
		if (state == "valid")
		{
			if (arg == "add")
			{	
				Bureaucrat q("Quentin", 150);
				std::cout << q << std::endl;
				for (int i = 0; i < loop; i++)
				{	
					q.incrementGrade();
					std::cout << q << std::endl;
				}
				
			}	
			else
			{
				Bureaucrat j("Jack", 1);
				std::cout << j << std::endl;
				for (int i = 0; i < loop; i++)
				{
					j.decrementGrade();
					std::cout << j << std::endl;
				}
			}

		}
		else
		{
			if (arg == "add")
				Bureaucrat b("Best", 0);
			else
				Bureaucrat w("Worst", 151);
		}	
	}

	catch (std::exception & e)
	{
			std::cout << "Exception caught: " << e.what() << std::endl;
			return 1;
	}

	std::cout << "No exception caught" << std::endl;
	
	return 0;
}
