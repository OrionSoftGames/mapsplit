// MapSplit by Orion_ [2010]

// http://onorisoft.free.fr/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------
// Endian Utils

void	WriteW(unsigned short word, FILE *f, int bige)
{
	if (bige)
		word = (word >> 8) | (word << 8);
	fwrite(&word, 2, 1, f);
}

void	WriteB(unsigned char byte, FILE *f)
{
	fwrite(&byte, 1, 1, f);
}

unsigned short ReadW(FILE *f, int bige)
{
	unsigned short word;
	fread(&word, 2, 1, f);
	if (bige)
		word = (word >> 8) | (word << 8);
	return (word);
}

unsigned char ReadB(FILE *f)
{
	unsigned char a;
	fread(&a, 1, 1, f);
	return (a);
}

//------------------------------------------

int	main(int argc, char *argv[])
{
	printf("MapSplit v2 - by Orion_ [2010/2015] http://onorisoft.free.fr/\n\n");

	if (argc > 5)
	{
		int		itilesize = atoi(argv[3]);
		int		otilesize = atoi(argv[4]);
		int		tilesetw = atoi(argv[5]);
		FILE	*f, *of;
		unsigned char	nlayer, sign[3], bige = 0, bytemode = 0;
		unsigned short	mw, mh, l, x, y, nx, ny, nsplit, tile, tx, ty, ntile;

		if (argc > 6)
		{
			if (strcmp(argv[6], "-bigendian") == 0)
				bige = 1;
			else if (strcmp(argv[6], "-byte") == 0)
				bytemode = 1;
		}

		f = fopen(argv[1], "rb");
		of = fopen(argv[2], "wb");
		if (f && of)
		{
			fread(sign, 1, 3, f);
			fread(&nlayer, 1, 1, f);
			mw = ReadW(f, bige);
			mh = ReadW(f, bige);

			if ((sign[0] == 'M') && (sign[1] == 'A') && (sign[2] == 'P'))
			{
				nsplit = itilesize / otilesize;
				if (nsplit)
				{
					unsigned short	nmw, nmh;

					nmw = mw * nsplit;
					nmh = mh * nsplit;
					fwrite(sign, 1, 3, of);
					fwrite(&nlayer, 1, 1, of);
					WriteW(nmw, of, bige);
					WriteW(nmh, of, bige);

					for (l = 0; l < nlayer; l++)
					{
						for (y = 0; y < mh; y++)
						{
							for (ny = 0; ny < nsplit; ny++)
							{
								for (x = 0; x < mw; x++)
								{
									if (bytemode)
										tile = ReadB(f);
									else
										tile = ReadW(f, bige);
									if (tile)
									{
										tx = (tile - 1) % (tilesetw / itilesize);
										ty = (tile - 1) / (tilesetw / itilesize);
										tx *= nsplit;
										ty *= nsplit;
									}
									for (nx = 0; nx < nsplit; nx++)
									{
										if (!tile)
										{
											if (bytemode)
												WriteB(tile, of);
											else
												WriteW(tile, of, bige);
										}
										else
										{
											ntile = 1 + (tx + nx) + ((ty + ny) * (tilesetw / otilesize));
											if (bytemode)
												WriteB(ntile, of);
											else
												WriteW(ntile, of, bige);
										}
									}
								}
								if (ny < nsplit - 1)
								{
									fseek(f, -(mw * (bytemode ? sizeof(char) : sizeof(short))), SEEK_CUR);
								}
							}
						}
					}
				}
				else
					printf("InputTileSize must be greater than OutputTileSize.\nOnly subdivision is supported\n");
			}
			else
				printf("Wrong map file format\n");

			fclose(of);
			fclose(f);
		}
		else
			printf("Cannot open or create files\n");
	}
	else
		printf("Usage:\nmapsplit mapfile.map outfile.map InputTileSize OutputTileSize TileSetWidth [-bigendian|-byte]\n\n");

	return (0);
}
