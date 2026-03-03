/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShrubberyCreationForm.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:03:19 by krfranco          #+#    #+#             */
/*   Updated: 2025/11/24 16:07:03 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ShrubberyCreationForm.hpp"
#include <fstream>

ShrubberyCreationForm::ShrubberyCreationForm(const std::string& t)
	: AForm("ShrubberyCreationForm", 145, 137)
	, target(t)
{
	// std::cout << "ShrubberyCreationForm: Default constructor called" << std::endl;
}

ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm& other)
	:AForm(other)
	, target(other.target)
{
	// std::cout << "ShrubberyCreationForm: Copy constructor called" << std::endl;
}

ShrubberyCreationForm& ShrubberyCreationForm::operator=(const ShrubberyCreationForm& other)
{
	// std::cout << "ShrubberyCreationForm: Copy assignment operator called" << std::endl;
	if (this != &other)
	{
		AForm::operator=(other);
	}
	return *this;
}

ShrubberyCreationForm::~ShrubberyCreationForm()
{
	// std::cout << "ShrubberyCreationForm: Destructor called" << std::endl;
}

void ShrubberyCreationForm::executeAction() const
{
	std::string tree =
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠒⠉⠉⠉⢢⠤⠤⡀⢀⣀⣀⠀⡠⠖⠋⠉⠉⠒⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣄⠀⠀⠀⠀⠀⠀⢀⠎⠀⠀⠀⠙⠄⠀⠀⠀⠀⠀⠈⡇⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠤⠐⠠⠄⡀⠀⠀⠀⢀⠄⠈⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠈⡇⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⡄⠀⠀⠀⠀⠀⠹⠔⠒⠢⠇⠀⠀⠀⠀⠀⠀⠠⠤⡖⠁⠘⡄⠀⠀⠀⠀⣠⣃⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⢀⡠⠤⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠃⠀⠀⠀⠀⠰⡄⠀⠀⠈⠁⠀⠙⣄⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⡔⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⠁⠀⠀⠀⠀⠀⠀⡜⠂⠲⠀⠀⠀\n"
"⠀⠀⠀⠀⠸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⠉⠀⠉⢢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣞⠀⠀⠘⠒⠂⡀\n"
"⠀⢀⡠⠐⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠁⠘⠂⠈⠆⠀⠀⠀⠀⠀⠀⠈⡆⠀⠀⠀⠀⡝\n"
"⢠⠋⠀⠀⠀⡠⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⢄⣀⣀⡠⡂⠀⠀⠀⠠⡄⠀⠀⢀⡀⠀⢀⡰⠃⠀⠀⠀⠺⣅\n"
"⡇⠀⠀⠀⠀⠣⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⡄⠀⠰⣀⡠⣀⠀⢀⣠⠎⠉⣹⣀⠀⠀⠀⠀⠀⣸\n"
"⡇⠀⠀⠀⠀⢰⡁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⣤⢷⠒⠂⠐⡎⠀⠀⠀⠀⠀⠀⠉⠁⠀⠀⠀⠁⠀⠉⢢⠀⢰⠊⠁\n"
"⠘⢤⡤⠀⠀⠀⠑⠂⠐⢆⠀⣄⣀⣀⠀⠀⡀⢀⡴⠀⠀⡧⣀⠜⠑⠂⢺⢳⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡜⠐⠃⠀⠀\n"
"⠀⡎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⢎⠀⠡⠀⠀⠱⡀⣠⠀⠀⡏⠈⢱⢲⠂⠀⠀⠀⠀⠀⠀⠀⠀⠱⠀⠀⠀⠀\n"
"⠀⠱⣄⡀⣠⠃⠀⠀⠀⠀⠀⠀⠀⠉⢫⣁⢫⡳⢄⢣⠀⠀⠉⠀⠀⢰⠇⢀⠇⢾⡻⠖⢖⠁⠀⡀⡀⠀⢀⡼⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠸⡀⠀⠀⠀⠐⣄⣀⣀⡠⠎⠓⢯⡀⠀⢸⠀⠀⠀⠀⠀⣸⢁⠎⡴⡋⠀⠀⠀⡉⠉⠀⠈⡏⠁⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠉⠒⠒⠒⠋⠀⠀⠀⠀⠀⠀⠀⠹⡄⠀⠀⠀⠀⠀⠀⣿⢏⡾⠝⠓⠢⠤⠊⠈⠑⠒⠊⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣧⠀⠀⠀⡠⠄⣀⣿⡎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⢸⡜⣉⢿⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡿⠀⠀⠸⡱⡻⡸⢸⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢤⡤⢄⡀⠀⢀⣀⣀⢇⠀⠀⠀⠉⠈⠀⡎⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠠⢤⡤⠀⢘⡆⠈⠋⣑⣽⣾⠌⠀⠀⠀⠀⠀⠀⠣⣸⣄⠀⢹⡅⠀⠔⠪⠿⢤⣤⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠐⠈⠉⠀⢀⣤⣖⣋⣁⣀⣋⣀⣀⠀⠒⠒⠄⠀⠀⠈⠈⠓⠦⢤⣀⠀⠀⠀⠀⠀⠑⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠒⠦⣄⣸⡟⠒⠒⠒⠒⠒⠚⠛⠀⠀⠀⠀⠀⢰⣶⣶⣶⣶⣶\n";

	std::string file = this->target + "_shrubbery";
	std::ofstream ofs(file.c_str());
	if (!ofs)
	{
		std::cerr << "Error: could not create file " << file << std::endl;
		return;
	}
	for (int i = 0; i < 3; i++)
	{	
		ofs << tree;
		ofs << '\n';
	}

	ofs.close();
}
