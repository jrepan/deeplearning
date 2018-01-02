#include <cassert>
#include <algorithm>
#include <iostream>
#include <glob.h>
#include <Magick++.h> 
using namespace std;
using namespace Magick; 

Quantum calculateMedian(Quantum *array, size_t count)
{
	sort(array, array + count);
	if (count % 2 == 0)
		return array[count / 2];
	else
		return (array[count / 2] + array[count / 2 + 1]) / 2;
}

int main()
{
	glob_t matches;
	int n = glob("match_*", 0, NULL, &matches);
	assert(n == 0);
	for (size_t i = 0; i < matches.gl_pathc; i++)
	{
		const string pattern = "/out_*.png";
		const string medial_filename = "/median.png";
		string match_pattern = matches.gl_pathv[i] + pattern;
		glob_t filenames;
		n = glob(match_pattern.c_str(), GLOB_NOSORT, NULL, &filenames);
		assert(n == 0);
		assert(filenames.gl_pathc > 0);
		Image *images[filenames.gl_pathc];
		for (size_t j = 0; j < filenames.gl_pathc; j++)
			images[j] = new Image(filenames.gl_pathv[j]);
		Geometry size = images[0]->size();

		Image median(size, Color("black"));
		median.modifyImage();
		PixelPacket* pixels = median.getPixels(0, 0, size.width(), size.height());
		Quantum reds  [filenames.gl_pathc];
		Quantum greens[filenames.gl_pathc];
		Quantum blues [filenames.gl_pathc];
		for (size_t x = 0; x < size.width(); x++)
			for (size_t y = 0; y < size.height(); y++)
			{
				for (size_t j = 0; j < filenames.gl_pathc; j++)
				{
					Color color = images[j]->pixelColor(x, y);
					reds[j]   = color.redQuantum();
					greens[j] = color.greenQuantum();
					blues[j]  = color.blueQuantum();
				}
				Quantum red   = calculateMedian(reds,   filenames.gl_pathc);
				Quantum green = calculateMedian(greens, filenames.gl_pathc);
				Quantum blue  = calculateMedian(blues,  filenames.gl_pathc);
				pixels[x + y * size.width()] = Color(red, green, blue);
			}
		median.syncPixels();

		for (size_t j = 0; j < filenames.gl_pathc; j++)
			delete images[j];
		globfree(&filenames);
		median.write(matches.gl_pathv[i] + medial_filename);
		cout << matches.gl_pathv[i] << " done" << endl;
	}
	globfree(&matches);
	return 0;
}
