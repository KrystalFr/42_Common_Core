/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bureaucrat.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 02:16:31 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 17:01:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Bureaucrat.hpp"
#include "../includes/AForm.hpp"

Bureaucrat::Bureaucrat(const std::string& n, int g) : name(n), grade(g)
{
	std::cout << "Bureaucrat: Default constructor called" << std::endl;
	if (grade < 1)
		throw GradeTooHighException();
	if (grade > 150)
		throw GradeTooLowException();
}

Bureaucrat::Bureaucrat(const Bureaucrat& other)
{
	std::cout << "Bureaucrat: Copy constructor called" << std::endl;
	*this = other;
}

Bureaucrat& Bureaucrat::operator=(const Bureaucrat& other)
{
	std::cout << "Bureaucrat: Copy assignment operator called" << std::endl;
	if (this != &other)
		this->grade = other.grade;
	return *this;
}

Bureaucrat::~Bureaucrat()
{
	std::cout << "Bureaucrat: Destructor called" << std::endl;
}

std::string Bureaucrat::getName() const
{
	return this->name;
}

int Bureaucrat::getGrade() const
{
	return this->grade;
}

void Bureaucrat::incrementGrade()
{
	this->grade--;
	if (grade < 1)
		throw GradeTooHighException();
}

void Bureaucrat::decrementGrade()
{
	this->grade++;
	if (grade > 150)
		throw GradeTooLowException();
}

void Bureaucrat::signForm(AForm& f)
{
	try
	{
		if (f.beSigned(*this))
			std::cout << this->getName() << " signed " << f.getName() << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cout << this->getName() << " couldn't sign "
					<< f.getName() << " because: sign" << e.what() << std::endl;
	}
}

void Bureaucrat::ExecuteForm(AForm const & f)
{
	try
	{
		f.execute(*this);
		std::cout << this->getName() << " executed " << f.getName() << std::endl;
	}
	catch (const std::exception & e)
	{
		std::cout << this->getName() << " couldn't execute "
					<< f.getName() << " because: exec" << e.what() << std::endl;
	}
}

const char* Bureaucrat::GradeTooHighException::what() const throw(){
	return "Grade too high";
}

const char* Bureaucrat::GradeTooLowException::what() const throw(){
	return "Grade too low";
}

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b)
{
	os << b.getName() << ", bureaucrat grade " << b.getGrade();
	return os;
}

