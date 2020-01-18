/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_hdr_list.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/18 13:15:16 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 13:16:35 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int		free_hdr_list(t_ft_nm_hdrinfo *hdrinfo)
{
	if (hdrinfo)
		free_hdr_list(hdrinfo->next);
	free(hdrinfo);
	return (1);
}
