/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AForm.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 14:41:31 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 14:39:51 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/AForm.hpp"
#include "../includes/Bureaucrat.hpp"

AForm::AForm(const std::string& n, const int sg, const int eg)
	: name(n)
	, signGrade(sg)
	, execGrade(eg)
	, state(false)
{
	std::cout << "Form: Default constructor called" << std::endl;
	if (signGrade < 1 || execGrade < 1)
		throw GradeTooHighException();
	if (signGrade > 150 || execGrade > 150)
		throw GradeTooLowException();
}

AForm::AForm(const AForm& other)
    : name(other.name)
    , signGrade(other.signGrade)
    , execGrade(other.execGrade)
    , state(other.state)
{
	std::cout << "Form: Copy constructor called" << std::endl;
}

AForm& AForm::operator=(const AForm& other)
{
	std::cout << "Form: Copy assignement operator called" << std::endl;
	if (this != &other)
		this->state = other.state;
	return *this;
}

AForm::~AForm()
{
	std::cout << "Form: Destructor called" << std::endl;
}

std::string AForm::getName() const
{
	return this->name;
}

int AForm::getsGrade() const
{
	return this->signGrade;
}

int AForm::geteGrade() const
{
	return this->execGrade;
}

bool AForm::isSigned() const
{
	return this->state;
}

bool AForm::beSigned(const Bureaucrat& b)
{
	if (b.getGrade() > this->signGrade)
		throw GradeTooLowException();
	if (!this->state)
	{
		this->state = true;
		return true;
	}
	else
	{
		std::cout << b.getName() << " couldn’t sign " << this->name << " because: it's already signed" << std::endl;
		return false;
	}

}

const char* AForm::GradeTooLowException::what() const throw(){
	return "Grade too low";
}

const char* AForm::GradeTooHighException::what() const throw(){
	return "Grade too high";
}

const char* AForm::NotSignedException::what() const throw(){
	return "Form is not signed";
}

std::ostream& operator<<(std::ostream& os, const AForm& f)
{
	std::string state;
	if (f.isSigned())
		state = "signed";
	else
		state = "unsigned";

	os << f.getName() << " form is " << state << ".\n"
		<< "Grade of " << f.getsGrade() << " required to sign\n"
		<< "Grade of " << f.geteGrade() << " required to execute";
	return os;
}

void AForm::execute(Bureaucrat const & executor) const
{
	if (!this->isSigned())
		throw NotSignedException();
	if (executor.getGrade() > this->geteGrade())
		throw GradeTooLowException();
	this->executeAction();
}