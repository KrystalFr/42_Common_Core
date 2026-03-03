/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 03:24:42 by krfranco          #+#    #+#             */
/*   Updated: 2025/11/11 19:58:39 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Bureaucrat.hpp"
#include "includes/Form.hpp"

//Bureaucrat: name, grade
//Form: name, signGrade, execGrade
int main()
{
	try
	{	
		Bureaucrat good("Jean Parfait", 1);
		Bureaucrat mid("Jean Padeursup", 74);
		Bureaucrat bad("Jean Mediocre", 149);

		Form important("Money", 10, 75);
		Form useless("Coffee", 75, 150);

		std::cout << std::endl;

		std::cout << good << std::endl;
		std::cout << mid << std::endl;
		std::cout << bad << std::endl;

		std::cout << std::endl;

		std::cout << important << std::endl;
		std::cout << std::endl;
		std::cout << useless << std::endl;

		std::cout << std::endl;

		bad.signForm(important);
		mid.signForm(important);
		good.signForm(important);
		
		std::cout << std::endl;

		bad.signForm(useless);
		mid.signForm(useless);
		good.signForm(useless);
		
		std::cout << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cout << "Invalid declaration: " << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}
