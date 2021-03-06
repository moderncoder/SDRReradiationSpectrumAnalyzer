#define _ITERATOR_DEBUG_LEVEL 0
#include <math.h>
#include <algorithm>
#include "glew-2.1.0/include/GL/glew.h"
#include <GL/glut.h>
////#include "glextension/glextension.h"                // helper for OpenGL extensions
#include "Graph.h"
#include "ArrayUtilities.h"
#include "DeviceReceiver.h"

GraphDataSeries::GraphDataSeries(void* parentGraph)
{	
	this->parentGraph = parentGraph;

	bufferSize = BUFFER_START_SIZE;

	//vertices = new VectorPtr[bufferSize];

	vertices = new VectorPtr[((Graph *)parentGraph)->maxDepth];

	////for (int i = 0; i < bufferSize; i++)
		////vertices[i] = NULL;	

	/*////for (int i = 0; i < ((Graph *)parentGraph)->maxDepth; i++)
		vertices[i] = new Vector[((Graph *)parentGraph)->verticesCount];
		*/

	for (int i = 0; i < ((Graph *)parentGraph)->maxDepth; i++)
		vertices[i] = new Vector[((Graph *)parentGraph)->verticesCount];

	GenerateSignalStrengthColors();


	verticesBufferSize = ((Graph *)parentGraph)->verticesCount * ((Graph *)parentGraph)->maxDepth;
	colorsBufferSize = verticesBufferSize;
	////normalsBufferSize = ((Graph *)parentGraph)->verticesCount * ((Graph *)parentGraph)->maxDepth;

	trianglesBufferSize = ((Graph *)parentGraph)->verticesCount * ((Graph *)parentGraph)->maxDepth * 2;
	indicesBufferSize = verticesBufferSize;
	indicesBufferCount = 0;

	
	verticesBuffer = new Vector[verticesBufferSize];
	bufferColors = new Color[colorsBufferSize];
	////normalsBuffer = new Vector[verticesBufferSize];

	////trianglesBuffer = new Triangle[trianglesBufferSize];
	////indicesBuffer = new GLuint[verticesBufferSize];
	
	
	
	glGenBuffers(1, &vboID);	

	size_t vSize = sizeof(Vector) * verticesBufferSize;
	size_t cSize = sizeof(Color) * colorsBufferSize;
	////size_t nSize = sizeof(Vector) * normalsBufferSize;


	// copy vertex attribs data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	////glBufferData(GL_ARRAY_BUFFER, sizeof(Vector) * verticesBufferSize + sizeof(Color) * colorsBufferSize + sizeof(Vector) * normalsBufferSize, NULL, GL_DYNAMIC_DRAW); // reserve space
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector) * verticesBufferSize + sizeof(Color) * colorsBufferSize, NULL, GL_DYNAMIC_DRAW); // reserve space
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, verticesBuffer);                  // copy verts at offset 0		
	////glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normalsBuffer);          // copy norms
	/////glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, cSize, bufferColors);          // copy cols
	glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, bufferColors);          // copy cols

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	delete [] verticesBuffer;
	delete [] bufferColors;
	////delete[] normalsBuffer;

}

void GraphDataSeries::GenerateSignalStrengthColors()
{
	uint8_t stage = 0;

	float r = 255, g = 0, b = 0;

	uint32_t colorIndex = 0;
	
	while (stage != 4)
	{
		colors[colorIndex].r = r / 255;
		colors[colorIndex].g = g / 255;
		colors[colorIndex].b = b / 255;

		colorIndex++;

		switch (stage)
		{
		case (0):
			g++;

			if (g == 255)
				stage++;
			break;


		case (1):
			r--;

			if (r == 0)
				stage++;
			break;


		case (2):
			b++;

			if (b == 255)
				stage++;
			break;

		case (3):
			g--;

			if (g == 0)
				stage++;
			break;
		}
	}
}

void GraphDataSeries::IncCircularBufferIndices()
{
	currentZIndex++;	

	if (currentZIndex >= ((Graph *)parentGraph)->maxDepth)
		currentZIndex = 0;

	if (currentZIndex == startZIndex)
	{
		startZIndex++;

		if (startZIndex >= ((Graph *)parentGraph)->maxDepth)
			startZIndex = 0;
	}
}


/*////uint32_t GraphDataSeries::SetData(uint8_t* data, uint32_t dataLength, uint32_t graphDataLength, bool complex, double iOffset, double qOffset, bool swapIQ)
{
	double x, y, z;
	uint32_t j = 0;

	uint8_t inc;

	if (complex)
		inc = 2;
	else
		inc = 1;


	for (uint32_t i = 0; i < length; i += inc)
	{
		x = i;						

		if (!swapIQ)
		{
			z = data[i];
			y = data[i + 1];
		}
		else
		{
			y = data[i];
			z = data[i + 1];
		}

		z += iOffset;
		y += qOffset;


		AddPoint(j, y, z, j);

		j++;
	}

	IncCircularBufferIndices();

	return length;
}

uint32_t GraphDataSeries::SetData(double* data, uint32_t dataLength, uint32_t graphDataLength, bool complex, double iOffset, double qOffset, bool swapIQ)
{
	double x, y, z;
	double magnitude;

	uint32_t j = 0;

	if (complex)
		dataLength >> 1;

	float resolution = graphDataLength / dataLength;

	////if (length<8000 || !((Graph *)parentGraph)->graph_3D)
	////if (length<8000 || !(style == GraphStyle::Graph3D))
	////resolution = 1;

	float inc = 1 / resolution;

	double max = -999999999;
	double value;


	uint32_t endIndex, maxIndex;

	for (uint32_t i = 0; i < dataLength; i += inc)
	{
		endIndex = i + inc;
		for (uint32_t k = i; k < endIndex && k < dataLength; k++)
		{
			if (!swapIQ)
			{
				z = data[k][0];
				y = data[k][1];
			}
			else
			{
				y = data[k][0];
				z = data[k][1];
			}


			value = std::max(abs(y), abs(z));

			////value = abs(z));

			if (value > max || k == i)
			{
				max = value;
				maxIndex = k;
			}
		}

		if (!swapIQ)
		{
			z = data[maxIndex][0];
			y = data[maxIndex][1];
		}
		else
		{
			y = data[maxIndex][0];
			z = data[maxIndex][1];
		}

		z += iOffset;
		y += qOffset;


		AddPoint(j, y, z, j);

		j++;
	}

	IncCircularBufferIndices();

	return dataLength;
}
*/


uint32_t GraphDataSeries::SetData(void* data, uint32_t dataLength, bool complex, double iOffset, double qOffset, bool swapIQ, SignalProcessingUtilities::DataType dataType)
{
	double x, y, z;
	double magnitude;

	uint32_t j = 0;

	if (complex && (dataType == SignalProcessingUtilities::DataType::UINT8_T || dataType == SignalProcessingUtilities::DataType::DOUBLE))
	{
		dataLength = dataLength >> 1;
	}


	float resolution = (float) maxResolution / dataLength;

	////if (length<8000 || !((Graph *)parentGraph)->graph_3D)
 	
	if (dataLength < 10 || !(style == GraphStyle::Graph3D))
		resolution = 1;
		

	float inc = 1 / resolution;

	double max = -999999999;
	double value;

	SignalProcessingUtilities::IQ iq, maxIQ;
	
	uint32_t endIndex, maxIndex;

	for (double i = 0; i < dataLength; i+=inc)
	{		
		maxIQ.I = 0;
		maxIQ.Q = 0;

		endIndex = i + inc;
		
		for (uint32_t k = i; (k < endIndex && k < dataLength) || (k == (uint32_t) i && inc < 1); k++)
		{			
			switch (dataType)
			{
				case(SignalProcessingUtilities::DataType::UINT8_T):
					iq = SignalProcessingUtilities::GetIQFromData((uint8_t *)data, k);
				break;
				case(SignalProcessingUtilities::DataType::DOUBLE):					
					iq = SignalProcessingUtilities::GetIQFromData((double *)data, k);
				break;
				case(SignalProcessingUtilities::DataType::FFTW_COMPLEX):
					iq = SignalProcessingUtilities::GetIQFromData((fftw_complex *)data, k);
				break;
			}			

			if (swapIQ)			
			{
				iq.Swap();				
			}


			//!average			
			////value = std::max(abs(iq.Q), abs(iq.I));
			/*////value = iq.Q;
			
			if (value > max || k == i)
			{
				max = value;
				maxIndex = k;
				maxIQ = iq;
			}
			*/

			//average
			maxIQ.I += iq.I;
			maxIQ.Q += iq.Q;
		}

		if (inc > 1)
		{
			//average
			maxIQ.I /= inc;
			maxIQ.Q /= inc;
		}
				
		maxIQ.I += iOffset;
		maxIQ.Q += qOffset;
		
		AddPoint(j, maxIQ.Q, maxIQ.I, j);

		j++;
	}

	IncCircularBufferIndices();

	return dataLength;
}

double GraphDataSeries::GetAvgValueForIndex(uint8_t index, uint32_t count)
{
	if (count == 0 || count > ((Graph *)parentGraph)->maxDepth)
		count = ((Graph *)parentGraph)->maxDepth;

	int32_t zIndex = currentZIndex;

	double total = 0, value;

	uint32_t zIndexCount = 0;
	do
	{
		zIndex--;

		if (zIndex < 0)
			zIndex = ((Graph *)parentGraph)->maxDepth - 1;

		////if (zIndexCount > 0)
		{
			value = vertices[zIndex][index].y;

			total += value;
		}

		zIndexCount++;		
	} while (zIndex != startZIndex && zIndexCount < count);

	value = total / zIndexCount;

	return value;
}

double GraphDataSeries::GetGradientForIndex(uint8_t index)
{
	int32_t zIndex = currentZIndex;
	int32_t zIndex2 = zIndex - 1;

	if (zIndex2 < 0)
		zIndex2 = ((Graph *)parentGraph)->maxDepth - 1;

	double totalGradient = 0, gradient;
	
	uint16_t gradientCount = 100;
	
	uint32_t zIndexCount = 0;
	do
	{
		zIndex--;
		zIndex2--;

		if (zIndex < 0)
			zIndex = ((Graph *)parentGraph)->maxDepth - 1;

		if (zIndex2 < 0)
			zIndex2 = ((Graph *)parentGraph)->maxDepth - 1;

		if (zIndexCount > 0)
		{
			gradient = vertices[zIndex][index].y - vertices[zIndex2][index].y;

			totalGradient += gradient;
		}
	
		zIndexCount++;

		if (zIndexCount >= gradientCount)
			break;

	} while (zIndex2 != startZIndex);

	gradient = totalGradient / zIndexCount;

	return gradient;
}

void GraphDataSeries::SetStyle(GraphStyle style)
{
	this->style = style;	
}

void GraphDataSeries::SetColor(float red, float green, float blue, float alpha)
{
	colorSet = true;

	color.r = red;
	color.g = green;
	color.b = blue;
}

void GraphDataSeries::SetDataWidth(uint32_t width)
{
	dataWidth = width;
}

uint32_t GraphDataSeries::AddPoint(double x, double y, double z, int32_t index)
{		
	if (index >= bufferSize)
	{
		vertices = ArrayUtilities::ResizeArray(&vertices[currentZIndex], bufferSize, bufferSize*1.1);

		bufferSize *= 1.1;
	}	

	if (index == -1)
		index = verticesCount;

	////if (vertices[currentZIndex][index])
	{
		vertices[currentZIndex][index].x = x;
		vertices[currentZIndex][index].y = y;
		vertices[currentZIndex][index].z = z;
	}
	/*////else
	{
		vertices[index] = new Vector(x, y, z);
	}*/

	if (index >= verticesCount)
		verticesCount = index + 1;

	return verticesCount;
}

Vector* GraphDataSeries::GetPoint(int32_t index)
{
	int32_t zIndexForPoint = currentZIndex - 1;

	if (zIndexForPoint < 0)
		zIndexForPoint = ((Graph *)parentGraph)->maxDepth - 1;

	return &vertices[zIndexForPoint][index];
}

uint32_t GraphDataSeries::AddColorToBuffer(float r, float g, float b)
{	
	bufferColors[colorsBufferCount].r = r;
	bufferColors[colorsBufferCount].g = g;
	bufferColors[colorsBufferCount].b = b;

	return colorsBufferCount++;
}


uint32_t GraphDataSeries::AddVertexToBuffer(float x, float y, float z)
{	
	verticesBuffer[verticesBufferCount].x = x;
	verticesBuffer[verticesBufferCount].y = y;
	verticesBuffer[verticesBufferCount].z = z;

	return verticesBufferCount++;
}

void GraphDataSeries::AddTriangleToBuffer(GLuint i1, GLuint i2, GLuint i3)
{
	trianglesBuffer[trianglesBufferCount].i1 = i1;
	trianglesBuffer[trianglesBufferCount].i2 = i2;
	trianglesBuffer[trianglesBufferCount].i3 = i3;

	/*////Vector normal = MathUtilities::CalcNormal(verticesBuffer[i1], verticesBuffer[i2], verticesBuffer[i3]);	

	normalsBuffer[i1].SetVector(normal.x, normal.y, normal.z);
	normalsBuffer[i2].SetVector(normal.x, normal.y, normal.z);
	normalsBuffer[i3].SetVector(normal.x, normal.y, normal.z);
	*/

	trianglesBufferCount++;
}

void GraphDataSeries::GenerateVertex(float x, float i, float q, float z, float zScale, float yScale, float viewYMin, bool useMagnitudes)
{	
	double value;
	
	if (useMagnitudes)
		value = sqrt(q*q + i*i);
	else
		value = q;
	
	////value = 0;
	
	if (colorSet)
	{
		////glColor3f(color.r, color.g, color.b);

		float normalizedRange = (value - dataSeriesMinMax.min) / dataSeriesMinMax.range;

		if (normalizedRange > 1)
			normalizedRange = 1;

		if (normalizedRange < 0)
			normalizedRange = 0;

		float  value = normalizedRange+0.5;

		float red, green, blue;

		red = green = blue = 0;

		if (color.r == 1)
			red = value;
		
		if (color.g == 1)
			green = value;

		if (color.b == 1)
			blue = value;

		AddColorToBuffer(red, green, blue);		

		////AddColorToBuffer(color.r, color.g, color.b);
	}
	else
	{
		float normalizedRange = (value - dataSeriesMinMax.min) / dataSeriesMinMax.range;

		if (normalizedRange > 1)
			normalizedRange = 1;

		if (normalizedRange < 0)
			normalizedRange = 0;

		int colorIndex = (int)((1 - normalizedRange) * (colorsCount - 1));

		////glColor3f(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b);

		AddColorToBuffer(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b);
	}

	////glVertex3f(x, (value - viewYMin) * yScale, z * zScale);	
	AddVertexToBuffer(x, (value - viewYMin) * yScale, z * zScale);
}

void GraphDataSeries::GenerateVertex2(float x, float i, float q, float z, float zScale, float yScale, float viewYMin, bool useMagnitudes)
{
	double value;

	if (useMagnitudes)
		value = sqrt(q*q + i*i);
	else
		value = q;

	if (colorSet)
	{
		glColor3f(color.r, color.g, color.b);
	}
	else
	{
		float normalizedRange = (value - dataSeriesMinMax.min) / dataSeriesMinMax.range;

		if (normalizedRange > 1)
			normalizedRange = 1;

		if (normalizedRange < 0)
			normalizedRange = 0;

		int colorIndex = (int)((1 - normalizedRange) * (colorsCount - 1));

		glColor3f(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b);		
	}

	glVertex3f(x, (value - viewYMin) * yScale, z * zScale);		
}

void GraphDataSeries::Draw(uint32_t startDataIndex, uint32_t endIndex, float viewYMin, float viewYMax, double scale, bool graphMagnitude)
{
	if (!visible)
		return;
	/*////
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

	// enable vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// before draw, specify vertex and index arrays with their offsets
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glNormalPointer(GL_FLOAT, 0, (void*)sizeof(verticesBuffer));
	glColorPointer(3, GL_FLOAT, 0, (void*)(sizeof(verticesBuffer) + sizeof(bufferNormals)));
	
	

	glDrawElements(GL_TRIANGLES,            // primitive type
		36,                      // # of indices
		GL_UNSIGNED_INT,         // data type
		(void*)0);               // ptr to indices

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// it is good idea to release VBOs with ID 0 after use.
	// Once bound with 0, all pointers in gl*Pointer() behave as real
	// pointer, so, normal vertex array operations are re-activated
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	*/

	if (style == GraphStyle::Graph3D)
	{
		Draw3D(startDataIndex, endIndex, viewYMin, viewYMax, scale, graphMagnitude);
		////Draw3D_2(startDataIndex, endIndex, viewYMin, viewYMax, scale, graphMagnitude);
	}
	else
		Draw2D(startDataIndex, endIndex, viewYMin, viewYMax, scale, graphMagnitude);
	////Draw2D_2(startDataIndex, endIndex, viewYMin, viewYMax, scale, graphMagnitude);
}

void GraphDataSeries::Draw2D(uint32_t startDataIndex, uint32_t endIndex, float viewYMin, float viewYMax, double scale, bool graphMagnitude)
{
	if (iboID == 999999999)
	{
		glGenBuffers(1, &iboID);

		// copy index data to VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesBufferSize, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	if (endIndex == 0 || endIndex > verticesCount)
		endIndex = verticesCount;

	if (startDataIndex == endIndex)
		endIndex++;

	uint32_t length = endIndex - startDataIndex;

	GLfloat zIndexCount = 0;

	if (startDataIndex < endIndex && endIndex - startDataIndex >= 2)
	{
		double yValue, zValue, magnitude;

		if (colorSet)
			glColor3f(color.r, color.g, color.b);

		glLineWidth(2);

		uint32_t endPosX = ((Graph *)parentGraph)->width;
		uint32_t dataLength = endIndex - startDataIndex;

		double xInc = ((Graph *)parentGraph)->width / dataLength;
		float x = 0;

		verticesBufferCount = 0;
		indicesBufferCount = 0;

		////if (scale == 1)
		{
			/*////dataSeriesMinMax = GetMinMax(startDataIndex, endIndex);

			if (viewYMax == 0)
				viewYMax = dataSeriesMinMax.max;
			*/

			////scale = ((Graph *)parentGraph)->height / (viewYMax - viewYMin);
			////scale = ((Graph *)parentGraph)->height / (viewYMax - viewYMin);
			////scale = 233.3;

			////glBegin(GL_LINE_STRIP);

			glBindBuffer(GL_ARRAY_BUFFER, vboID);
			verticesBuffer = (Vector *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			////bufferColors = (Color *)&verticesBuffer[verticesBufferSize];

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
			indicesBuffer = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);

			for (int i = startDataIndex; i < endIndex; i++)
			{
				yValue = (vertices[0][i].y - viewYMin) * scale;
				zValue = vertices[0][i].z * scale;

				////zValue = 0;

				////glVertex3f(x, yValue, zValue);
				AddVertexToBuffer(x, yValue, zValue);

				indicesBuffer[indicesBufferCount] = indicesBufferCount;

				indicesBufferCount++;

				x += xInc;
			}

			////glEnd();

			glUnmapBuffer(GL_ARRAY_BUFFER);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);


			if (verticesBufferCount > 0)
			{	
				/*////
				// copy vertex attribs data to VBO
				glBindBuffer(GL_ARRAY_BUFFER, vboID);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector) * verticesBufferSize, verticesBuffer); // copy verts at offset 0	
																																								// copy index data to VBO
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);				
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLuint) * indicesBufferCount, indicesBuffer);
				*/

				glBindBuffer(GL_ARRAY_BUFFER, vboID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

				// enable vertex arrays
				glEnableClientState(GL_VERTEX_ARRAY);
				////glEnableClientState(GL_COLOR_ARRAY);


				// before draw, specify vertex and index arrays with their offsets
				glVertexPointer(3, GL_DOUBLE, 0, 0);
				////glNormalPointer(GL_FLOAT, 0, (void*) sizeof(Vector) * verticesBufferCount);
				////glColorPointer(4, GL_FLOAT, 0, (void*)(sizeof(Vector) * verticesBufferSize));

				glDrawElements(GL_LINE_STRIP,            // primitive type
					indicesBufferCount,                      // # of indices
					GL_UNSIGNED_INT,         // data type
					(void*)0);               // ptr to indices

				glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
				////glDisableClientState(GL_COLOR_ARRAY);


				// it is good idea to release VBOs with ID 0 after use.
				// Once bound with 0, all pointers in gl*Pointer() behave as real
				// pointer, so, normal vertex array operations are re-activated
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		////dataSeriesMinMax = GetMinMax();

		/*////
		if (scale == 1)
		{
		dataSeriesMinMax = GetMinMax();

		int32_t zIndex = currentZIndex + 1;

		do
		////for (int32_t zIndex = 0; zIndex < currentZIndex; zIndex++)
		{
		zIndex--;

		if (zIndex < 0)
		zIndex = ((Graph *)parentGraph)->maxDepth - 1;

		glBegin(GL_LINE_STRIP);

		x = startPosX;

		for (int i = startDataIndex; i < endIndex; i++)
		{
		yValue = vertices[zIndex][i].y;
		zValue = vertices[zIndex][i].z;

		glVertex3f(x, yValue, -zIndexCount * 10);

		x += xInc;
		}

		glEnd();

		zIndexCount++;
		} while (zIndex != startZIndex);
		}
		else
		{
		for (uint32_t zIndex = 0; zIndex < ((Graph *)parentGraph)->maxDepth; zIndex++)
		{
		glBegin(GL_LINE_STRIP);

		for (int i = startDataIndex; i < endIndex; i++)
		{
		////if (vertices[i])
		{
		yValue = vertices[zIndex][i].y * scale;
		zValue = vertices[zIndex][i].z * scale;

		glVertex3f(x, yValue, zValue);
		}

		x += xInc;
		}

		glEnd();
		}

		}*/


	}
}

void GraphDataSeries::Draw2D_2(uint32_t startDataIndex, uint32_t endIndex, float viewYMin, float viewYMax, double scale, bool graphMagnitude)
{
	if (endIndex == 0 || endIndex > verticesCount)
		endIndex = verticesCount;

	if (startDataIndex == endIndex)
		endIndex++;

	uint32_t length = endIndex - startDataIndex;

	GLfloat zIndexCount = 0;

	if (startDataIndex <endIndex && endIndex - startDataIndex >= 2)
	{
		double yValue, zValue, magnitude;

		if (colorSet)
			glColor3f(color.r, color.g, color.b);

		glLineWidth(2);
		
		uint32_t endPosX = ((Graph *)parentGraph)->width;
		uint32_t dataLength = endIndex - startDataIndex;

		double xInc = ((Graph *)parentGraph)->width / dataLength;
		float x = 0;

		////if (scale == 1)
		{
				dataSeriesMinMax = GetMinMax(startDataIndex, endIndex);
				
				if (viewYMax == 0)
					viewYMax = dataSeriesMinMax.max;

				scale = ((Graph *)parentGraph)->height / (viewYMax - viewYMin);

				glBegin(GL_LINE_STRIP);
				
				for (int i = startDataIndex; i < endIndex; i++)
				{
					yValue = (vertices[0][i].y - viewYMin) * scale;
					zValue = vertices[0][i].z * scale;

					////zValue = 0;

					glVertex3f(x, yValue, zValue);

					x += xInc;
				}

				glEnd();
		}

		////dataSeriesMinMax = GetMinMax();

		/*////
		if (scale == 1)
		{
			dataSeriesMinMax = GetMinMax();

			int32_t zIndex = currentZIndex + 1;

			do
				////for (int32_t zIndex = 0; zIndex < currentZIndex; zIndex++)
			{
				zIndex--;

				if (zIndex < 0)
					zIndex = ((Graph *)parentGraph)->maxDepth - 1;

				glBegin(GL_LINE_STRIP);

				x = startPosX;

				for (int i = startDataIndex; i < endIndex; i++)
				{
					yValue = vertices[zIndex][i].y;
					zValue = vertices[zIndex][i].z;

					glVertex3f(x, yValue, -zIndexCount * 10);
					
					x += xInc;
				}

				glEnd();

				zIndexCount++;
			} while (zIndex != startZIndex);
		}
		else
		{
			for (uint32_t zIndex = 0; zIndex < ((Graph *)parentGraph)->maxDepth; zIndex++)
			{
				glBegin(GL_LINE_STRIP);

				for (int i = startDataIndex; i < endIndex; i++)
				{
					////if (vertices[i])
					{
						yValue = vertices[zIndex][i].y * scale;
						zValue = vertices[zIndex][i].z * scale;

						glVertex3f(x, yValue, zValue);
					}

					x += xInc;
				}

				glEnd();
			}

		}*/


	}
}


void GraphDataSeries::Draw3D(uint32_t startDataIndex, uint32_t endIndex, float viewYMin, float viewYMax, double scale, bool graphMagnitude)
{
	if (iboID == 999999999)
	{
		glGenBuffers(1, &iboID);

		// copy index data to VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * trianglesBufferSize, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (endIndex == 0 || endIndex > verticesCount)
		endIndex = verticesCount;

	if (startDataIndex == endIndex)
		endIndex++;

	uint32_t length = endIndex - startDataIndex;
	GLfloat zIndexCount = 0, zIndexCount2 = 1;

	uint32_t dataLength = endIndex - startDataIndex;

	if (dataLength >= 2)
	{
		double yValue, zValue, magnitude;

		double normalizedRange = 0;

		glColor3f(color.r, color.g, color.b);
		////glLineWidth(2);


		uint32_t startPosX = 0;
		uint32_t endPosX = ((Graph *)parentGraph)->width;
		

		double xInc = ((Graph *)parentGraph)->width / (dataLength-1);
		float x;
		float x2;

		if (((Graph*)parentGraph)->wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		if (scale == 0)
		{
			if (!graphMagnitude)
				dataSeriesMinMax = GetMinMax(0, 0, true, false);
			else
				dataSeriesMinMax = GetMinMaxForMagnitudes(startDataIndex, endIndex);

			if (dataSeriesMinMax.range == 0)
				return;

			////dataSeriesMinMax.min = 0;
			////dataSeriesMinMax.max = 3.14;					

			if (viewYMin == 0)
				viewYMin = dataSeriesMinMax.min;

			if (viewYMax == 0)
				viewYMax = dataSeriesMinMax.max;

			if (viewYMax != viewYMin)
				scale = ((Graph *)parentGraph)->height / (viewYMax - viewYMin);
			else
				scale = 1;

			/*
			if (viewYMin == 0)
				viewYMin = 0;

			if (viewYMax == 0)
				viewYMax = 10;


			scale = 70;
			*/
		}
		else
		{
			dataSeriesMinMax.min = viewYMin;
			dataSeriesMinMax.max = viewYMax;

			dataSeriesMinMax.CalculateRange();

			if (dataSeriesMinMax.range == 0)
				return;
		}


		double zScale = 10;

		if (currentZIndex == startZIndex)
			return;

		int32_t zIndex = currentZIndex;
		int32_t zIndex2 = zIndex - 1;

		if (zIndex2 < 0)
			zIndex2 = ((Graph *)parentGraph)->maxDepth - 1;


		if (!(style == GraphStyle::Line3D || ((Graph *)parentGraph)->drawDepth <= 3))
		{
			verticesBufferCount = 0;
			colorsBufferCount = 0;

			trianglesBufferCount = 0;

			glBindBuffer(GL_ARRAY_BUFFER, vboID);
			verticesBuffer = (Vector *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			////normalsBuffer = (Vector *)&verticesBuffer[verticesBufferSize];
			////bufferColors = (Color *) &verticesBuffer[verticesBufferSize*2];
			bufferColors = (Color *)&verticesBuffer[verticesBufferSize];


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
			trianglesBuffer = (Triangle *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
		}
		else
		{
			endIndex--;
			glLineWidth(lineWidth);
		}

		double iOffset = 0;
		

		do
		{
			zIndex--;
			zIndex2--;

			if (zIndex < 0)
				zIndex = ((Graph *)parentGraph)->maxDepth - 1;

			if (zIndex2 < 0)
				zIndex2 = ((Graph *)parentGraph)->maxDepth - 1;

			if (zIndex == startZIndex)
				break;

			x = startPosX;

			if (style == GraphStyle::Line3D || ((Graph *)parentGraph)->drawDepth <= 3)
			{				
				for (int i = startDataIndex; i < endIndex; i++)
				{
					////if (i > startDataIndex)
					{
						glBegin(GL_LINES);

						GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
						GenerateVertex2(x + xInc, vertices[zIndex][i+1].z, vertices[zIndex][i+1].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
						////GenerateVertex2(x, vertices[zIndex2][i].z, vertices[zIndex2][i].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);

						glEnd();
					}

					x += xInc;
				}
			}
			else
			if (style == GraphStyle::Graph3D)
			{
				for (int i = startDataIndex; i < endIndex; i++)
				{
					GenerateVertex(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);

					if (i > startDataIndex && zIndexCount > 0)
					{
						iOffset = i - startDataIndex;

						AddTriangleToBuffer((zIndexCount - 1) * dataLength + iOffset, zIndexCount * dataLength + iOffset, (zIndexCount * dataLength) + iOffset - 1);
						AddTriangleToBuffer((zIndexCount * dataLength) + iOffset - 1, (zIndexCount - 1) * dataLength + iOffset - 1, (zIndexCount - 1) * dataLength + iOffset);
					}

					x += xInc;
				}
			}

			zIndexCount++;
		} while (zIndex2 != startZIndex && zIndexCount < ((Graph *)parentGraph)->drawDepth);


		if (!(style == GraphStyle::Line3D || ((Graph *)parentGraph)->drawDepth <= 3))
		{
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			if (trianglesBufferCount > 0)
			{
				size_t vSize = sizeof(Vector) * verticesBufferSize;
				////size_t nSize = sizeof(Vector) * normalsBufferSize;
				size_t cSize = sizeof(Color) * colorsBufferSize;


				// copy vertex attribs data to VBO
				/*////glBindBuffer(GL_ARRAY_BUFFER, vboID);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, verticesBuffer);                  // copy verts at offset 0
				glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, bufferColors);          // copy cols after norms

				// copy index data to VBO
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Triangle) * trianglesBufferCount, trianglesBuffer);
				*/


				////glBindBuffer(GL_ARRAY_BUFFER, vboID);
				////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);


				// enable vertex arrays
				glEnableClientState(GL_VERTEX_ARRAY);
				////glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, vboID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

				// before draw, specify vertex and index arrays with their offsets
				glVertexPointer(3, GL_DOUBLE, 0, 0);
				////glNormalPointer(GL_DOUBLE, 0, (void*) (vSize));
				////glColorPointer(4, GL_FLOAT, 0, (void*) (vSize + nSize));
				glColorPointer(4, GL_FLOAT, 0, (void*)vSize);


				glDrawElements(GL_TRIANGLES,            // primitive type
					trianglesBufferCount * 3,                      // # of indices
					GL_UNSIGNED_INT,         // data type
					(void*)0);               // ptr to indices

				glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
				////glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);


				// it is good idea to release VBOs with ID 0 after use.
				// Once bound with 0, all pointers in gl*Pointer() behave as real
				// pointer, so, normal vertex array operations are re-activated
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}
	}
}



void GraphDataSeries::Draw3D_2(uint32_t startDataIndex, uint32_t endIndex, float viewYMin, float viewYMax, double scale, bool graphMagnitude)
{	if (endIndex == 0 || endIndex > verticesCount)
		endIndex = verticesCount;

	if (startDataIndex == endIndex)
		endIndex++;

	uint32_t length = endIndex - startDataIndex;	
	GLfloat zIndexCount = 0, zIndexCount2 = 1;

	if (endIndex - startDataIndex >= 2)
	{
		double yValue, zValue, magnitude;

		double normalizedRange = 0;

		glColor3f(color.r, color.g, color.b);
		glLineWidth(2);


		uint32_t startPosX = 0;
		uint32_t endPosX = ((Graph *)parentGraph)->width;
		uint32_t dataLength = endIndex - startDataIndex;

		double xInc = ((Graph *)parentGraph)->width / dataLength;
		float x;
		float x2;

		if (((Graph*)parentGraph)->wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		if (scale == 1)
		{
			if (!graphMagnitude)
				dataSeriesMinMax = GetMinMax(0, 0, true, false);
			else
				dataSeriesMinMax = GetMinMaxForMagnitudes(startDataIndex, endIndex);

			if (viewYMin == 0)
				viewYMin = dataSeriesMinMax.min;

			////viewYMin *= 2;
			
			if (viewYMax == 0)
				viewYMax = dataSeriesMinMax.max;

			////viewYMin = 0;

			if (viewYMax != viewYMin)
				scale = ((Graph *)parentGraph)->height / (viewYMax - viewYMin);
			else
				scale = 1;
			
			double zScale = 10;

			if (currentZIndex == startZIndex)
				return;

			int32_t zIndex = currentZIndex;
			int32_t zIndex2 = zIndex - 1;

			if (zIndex2 < 0)
				zIndex2 = ((Graph *)parentGraph)->maxDepth - 1;

			do				
			{
				zIndex--;
				zIndex2--;

				if (zIndex < 0)
					zIndex = ((Graph *)parentGraph)->maxDepth - 1;

				if (zIndex2 < 0)
					zIndex2 = ((Graph *)parentGraph)->maxDepth - 1;

				if (zIndex == startZIndex)
					break;

				////glBegin(GL_LINE_STRIP);				

				x = startPosX;
				x2 = x + xInc;

				if (style == GraphStyle::Line3D)
				{
					glLineWidth(lineWidth);

					for (int i = startDataIndex; i < endIndex - 1; i++)
					{
						glBegin(GL_LINES);
						
						GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);						
						GenerateVertex2(x, vertices[zIndex2][i].z, vertices[zIndex2][i].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);

						glEnd();

						x += xInc;						
					}
				}
				else
					if (style == GraphStyle::Area3D)
					{		
						glDisable(GL_CULL_FACE);
						for (int i = startDataIndex; i < endIndex - 1; i++)
						{							
							glBegin(GL_TRIANGLES);

							GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
							GenerateVertex2(x, vertices[zIndex2][i].z, vertices[zIndex2][i].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);
							glVertex3f(x, 0, -zIndexCount2 * zScale);

							glVertex3f(x, 0, -zIndexCount2 * zScale);
							glVertex3f(x, 0, -zIndexCount * zScale);
							GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
							
							glEnd();

							x += xInc;							
						}
						glEnable(GL_CULL_FACE);
					}
					else if (style == GraphStyle::Graph3D)
					{
						glBegin(GL_TRIANGLES);


						if (zIndexCount == 0)
						{
							////glBegin(GL_TRIANGLES);

							for (int i = startDataIndex; i < endIndex - 1; i++)
							{							
								GenerateVertex2(x2, vertices[zIndex][i + 1].z, vertices[zIndex][i + 1].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
								GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
								glVertex3f(x, 0, 0);

								glVertex3f(x, 0, 0);
								glVertex3f(x2, 0, 0);
								GenerateVertex2(x2, vertices[zIndex][i + 1].z, vertices[zIndex][i + 1].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);																							

								x += xInc;
								x2 = x + xInc;
							}

							////glEnd();

							x = startPosX;
							x2 = x + xInc;
						}
						



						////glBegin(GL_TRIANGLES);

						GenerateVertex2(x, vertices[zIndex][0].z, vertices[zIndex][0].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
						GenerateVertex2(x, vertices[zIndex2][0].z, vertices[zIndex2][0].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);
						glVertex3f(x, 0, -zIndexCount2 * zScale);

						glVertex3f(x, 0, -zIndexCount2 * zScale);
						glVertex3f(x, 0, -zIndexCount * zScale);
						GenerateVertex2(x, vertices[zIndex][0].z, vertices[zIndex][0].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);

						////glEnd();

						int i;

						////glBegin(GL_TRIANGLES);

						for (i = startDataIndex; i < endIndex - 1; i++)
						{							
							GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
							GenerateVertex2(x2, vertices[zIndex][i + 1].z, vertices[zIndex][i + 1].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
							GenerateVertex2(x2, vertices[zIndex2][i + 1].z, vertices[zIndex2][i + 1].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);

							GenerateVertex2(x2, vertices[zIndex2][i + 1].z, vertices[zIndex2][i + 1].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);
							GenerateVertex2(x, vertices[zIndex2][i].z, vertices[zIndex2][i].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);
							GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);							

							x += xInc;
							x2 = x + xInc;
						}

						////glBegin(GL_TRIANGLES);												
						
						GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
						glVertex3f(x, 0, -zIndexCount * zScale);
						glVertex3f(x, 0, -zIndexCount2 * zScale);

						glVertex3f(x, 0, -zIndexCount2 * zScale);
						GenerateVertex2(x, vertices[zIndex2][i].z, vertices[zIndex2][i].y, -zIndexCount2, zScale, scale, viewYMin, graphMagnitude);
						GenerateVertex2(x, vertices[zIndex][i].z, vertices[zIndex][i].y, -zIndexCount, zScale, scale, viewYMin, graphMagnitude);
						
						////glEnd();


						glEnd();
					}

				zIndexCount++;
				zIndexCount2 = zIndexCount + 1;

			} while (zIndex2 != startZIndex);
		}
		else
		{
			for (uint32_t zIndex = 0; zIndex < ((Graph *)parentGraph)->maxDepth; zIndex++)
			{
				glBegin(GL_LINE_STRIP);

				for (int i = startDataIndex; i < endIndex; i++)
				{
					////if (vertices[i])
					{
						yValue = vertices[zIndex][i].y * scale;
						zValue = vertices[zIndex][i].z * scale;

						glVertex3f(x, yValue, zValue);
					}

					x += xInc;
				}

				glEnd();
			}

		}


	}
}




MinMax GraphDataSeries::GetMinMax(uint32_t startDataIndex, uint32_t endIndex, bool useY, bool useZ)
{
	double value, yValue, zValue;

	MinMax dataSeriesMinMax;

	int32_t zIndex = currentZIndex;

	if (endIndex == 0)
		endIndex = verticesCount;

	////for (uint32_t zIndex = 0; zIndex < currentZIndex; zIndex++)

	uint32_t zIndexCount = 0;
	do
	{
		zIndex--;

		if (zIndex < 0)
			zIndex = ((Graph *)parentGraph)->maxDepth - 1;

		////for (int i = 0; i < verticesCount; i++)
		for (int i = startDataIndex; i < endIndex; i++)
		{
			if (useY && useZ)
				value = std::min(vertices[zIndex][i].y, vertices[zIndex][i].z);
			else
				if (useY)
					value = vertices[zIndex][i].y;
				else
					if (useZ)
						value = vertices[zIndex][i].z;
					else
					{
						yValue = vertices[zIndex][i].y;
						zValue = vertices[zIndex][i].z;

						value = sqrt(yValue*yValue + zValue*zValue);
					}


			if (value < dataSeriesMinMax.min || dataSeriesMinMax.min == 999999999)
				dataSeriesMinMax.min = value;

			if (useY && useZ)
				value = std::max(vertices[zIndex][i].y, vertices[zIndex][i].z);			

			if (value > dataSeriesMinMax.max || dataSeriesMinMax.max == -999999999)
				dataSeriesMinMax.max = value;
		}

		zIndexCount++;

	} while (zIndex != startZIndex && zIndexCount <= ((Graph *)parentGraph)->drawDepth);

	dataSeriesMinMax.range = dataSeriesMinMax.max - dataSeriesMinMax.min;

	return dataSeriesMinMax;
}

MinMax GraphDataSeries::GetMinMaxForMagnitudes(uint32_t startDataIndex, uint32_t endIndex)
{
	double yValue, zValue, magnitude;

	MinMax dataSeriesMinMax;

	////if (currentZIndex == startZIndex)
		////return dataSeriesMinMax;

	if (startDataIndex > endIndex)
		return dataSeriesMinMax;

	int32_t zIndex = currentZIndex;

	if (endIndex == 0)
		endIndex = verticesCount;

	do
	{
		zIndex--;

		if (zIndex < 0)
			zIndex = ((Graph *)parentGraph)->maxDepth - 1;

		for (int i = startDataIndex; i < endIndex; i++)
		{
			yValue = vertices[zIndex][i].y;
			zValue = vertices[zIndex][i].z;

			magnitude = sqrt(yValue*yValue + zValue*zValue);

			/*////if (std::isinf(magnitude))
				magnitude = 0;

			if (magnitude>999999999)
				magnitude = 0;
				*/

			if (magnitude < dataSeriesMinMax.min || dataSeriesMinMax.min == 999999999)
				dataSeriesMinMax.min = magnitude;

			if (magnitude > dataSeriesMinMax.max || dataSeriesMinMax.max == -999999999)
				dataSeriesMinMax.max = magnitude;
		}
	} while (zIndex != startZIndex);

	dataSeriesMinMax.range = dataSeriesMinMax.max - dataSeriesMinMax.min;

	return dataSeriesMinMax;
}

GraphDataSeries::~GraphDataSeries()
{
	delete vertices;
}

uint32_t GraphDataSeries::BUFFER_START_SIZE = 20000;