/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/10 10:30:56 by jjaniec           #+#    #+#             */
/*   Updated: 2018/02/01 12:18:44 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int		ft_isalpha(int c)
{
	return (((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) ? (1) : (0));
}
