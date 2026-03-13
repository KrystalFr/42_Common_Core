/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Array.tpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 22:49:14 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/09 13:57:56 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template <typename T>
Array<T>::Array() : array(NULL), len(0) {}

template <typename T>
Array<T>::Array(const unsigned int n) : array(new T[n]), len(n) {}

template <typename T>
Array<T>::Array(const Array &other) : array(new T[other.len]), len(other.len)
{
	for (unsigned int i = 0; i < len; i++)
		array[i] = other.array[i];
}

template <typename T>
Array<T> &Array<T>::operator=(const Array &other)
{
	if (this != &other)
	{
		T *tmp_array = new T[other.len];

		for (unsigned int i = 0; i < other.len; i++)
			tmp_array[i] = other.array[i];
		delete[] array;
		array = tmp_array;
		len = other.len;
	}
	return *this;
}

template <typename T>
Array<T>::~Array()
{
	delete[] array;
}

template <typename T>
unsigned int Array<T>::size() const
{
	return len;
}

template <typename T>
T &Array<T>::operator[](unsigned int i)
{
	if (i >= len)
		throw OutOfBounds();
	return array[i];
}

template <typename T>
const T &Array<T>::operator[](unsigned int i) const
{
	if (i >= len)
		throw OutOfBounds();
	return array[i];
}

template <typename T>
const char *Array<T>::OutOfBounds::what() const throw()
{
	return "Index is out of bounds";
}