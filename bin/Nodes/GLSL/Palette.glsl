layout (std140) uniform PaletteBlock
{
	int paletteIndex;
	float ditherStrength;
} PaletteParam;

float find_closest(int x, int y, float c0)
{
	int dither[8][8] = {
		{ 0, 32, 8, 40, 2, 34, 10, 42}, /* 8x8 Bayer ordered dithering */
		{48, 16, 56, 24, 50, 18, 58, 26}, /* pattern. Each input pixel */
		{12, 44, 4, 36, 14, 46, 6, 38}, /* is scaled to the 0..63 range */
		{60, 28, 52, 20, 62, 30, 54, 22}, /* before looking in this table */
		{ 3, 35, 11, 43, 1, 33, 9, 41}, /* to determine the action. */
		{51, 19, 59, 27, 49, 17, 57, 25},
		{15, 47, 7, 39, 13, 45, 5, 37},
		{63, 31, 55, 23, 61, 29, 53, 21} }; 

	float limit = 0.0;
	if(x < 8)
	{
		limit = (dither[x][y]+1)/64.0;
	}

	if(c0 < limit)
		return 0.0;
	return 1.0;
}

vec4 hex(int u_color)
{
	float rValue = float((u_color&0xFF0000)>>16);
	float gValue = float((u_color&0xFF00)>>8);
	float bValue = float(u_color&0xFF);
	return vec4(rValue, gValue, bValue, 255.0) / 255.;
}

vec4 best4(vec4 original, int pal[4])
{
	int best = 0;
	for (int i = 1;i<4;i++)
	{
		if (length(hex(pal[i]) - original) < length(hex(pal[best]) - original))
			best = i;
	}
	
	return hex(pal[best]);
}

vec4 best16(vec4 original, int pal[16])
{
	int best = 0;
	for (int i = 1;i<16;i++)
	{
		if (length(hex(pal[i]) - original) < length(hex(pal[best]) - original))
			best = i;
	}
	
	return hex(pal[best]);
}

vec4 GetCGA(vec4 original, int index)
{
	int pal[4];
	pal[0] = 0x000000;
	switch (index)
	{
	case 0:
		pal[1] = 0x00AAAA;
		pal[2] = 0xAA00AA;
		pal[3] = 0xAAAAAA;
		break;
	case 1:
		pal[1] = 0x00AAAA;
		pal[2] = 0xAA0000;
		pal[3] = 0xAAAAAA;
		break;
	case 2:
		pal[1] = 0x00AA00;
		pal[2] = 0xAA0000;
		pal[3] = 0xAA5500;
		break;
	case 3:
		pal[1] = 0x55FFFF;
		pal[2] = 0xFF55FF;
		pal[3] = 0xFFFFFF;
		break;
	case 4:
		pal[1] = 0x55FFFF;
		pal[2] = 0xFF5555;
		pal[3] = 0xFFFFFF;
		break;
	case 5:
		pal[1] = 0x55FF55;
		pal[2] = 0xFF5555;
		pal[3] = 0xFFFF55;
		break;
	}
	return best4(original, pal);
}

vec4 GetEGA(vec4 original)
{
	int pal[16];
	pal[0] = 0x000000;
	pal[1] = 0x0000AA;
	pal[2] = 0x00AA00;
	pal[3] = 0x00AAAA;
	
	pal[4] = 0xAA0000;
	pal[5] = 0xAA00AA;
	pal[6] = 0xAA5500;
	pal[7] = 0xAAAAAA;
	
	pal[8] = 0x555555;
	pal[9] = 0x5555FF;
	pal[10] = 0x55FF55;
	pal[11] = 0x55FFFF;
	
	pal[12] = 0xFF5555;
	pal[13] = 0xFF55FF;
	pal[14] = 0xFFFF55;
	pal[15] = 0xFFFFFF;
	
	return best16(original, pal);
}

vec4 GetGameBoy(vec4 original)
{
	int pal[4];
	pal[0] = 0x0f380f;
	pal[1] = 0x306230;
	pal[2] = 0x8bac0f;
	pal[3] = 0x9bbc0f;
	
	return best4(original, pal);
}

vec4 GetPico8(vec4 original)
{
	int pal[16];
	pal[0] = 0x000000;
	pal[1] = 0x5F574F;
	pal[2] = 0xC2C3C7;
	pal[3] = 0xFFF1E8;
	
	pal[4] = 0xFFEC27;
	pal[5] = 0xFFA300;
	pal[6] = 0xFFCCAA;
	pal[7] = 0xAB5236;
	
	pal[8] = 0xFF77A8;
	pal[9] = 0xFF004D;
	pal[10] = 0x83769C;
	pal[11] = 0x7E2553;
	
	pal[12] = 0x29ADDD;
	pal[13] = 0x1D2B53;
	pal[14] = 0x008751;
	pal[15] = 0x00E436;
	
	return best16(original, pal);
}

vec4 GetC64(vec4 original)
{
	int pal[16];
	pal[0] = 0x000000;
	pal[1] = 0xFFFFFF;
	pal[2] = 0x880000;
	pal[3] = 0xAAFFEE;
	
	pal[4] = 0xCC44CC;
	pal[5] = 0x00CC55;
	pal[6] = 0x0000AA;
	pal[7] = 0xEEEE77;
	
	pal[8] = 0xDD8855;
	pal[9] = 0x664400;
	pal[10] = 0xFF7777;
	pal[11] = 0x333333;
	
	pal[12] = 0x777777;
	pal[13] = 0xAAFF66;
	pal[14] = 0x0088FF;
	pal[15] = 0xBBBBBB;
	
	return best16(original, pal);
}

vec4 Palette()
{
	vec4 color = texture(Sampler0, vUV);

	vec3 rgb = color.rgb;
	vec2 xy = gl_FragCoord.xy;
	int x = int(mod(xy.x, 8));
	int y = int(mod(xy.y, 8));

	vec3 ditheredRGB;
	ditheredRGB.r = find_closest(x, y, rgb.r);
	ditheredRGB.g = find_closest(x, y, rgb.g);
	ditheredRGB.b = find_closest(x, y, rgb.b);
	float r = 0.25;
	color = vec4(mix(color.rgb, ditheredRGB, PaletteParam.ditherStrength), 1.0);
	

	if (PaletteParam.paletteIndex < 6)
		return GetCGA(color, PaletteParam.paletteIndex);
	if (PaletteParam.paletteIndex < 7)
		return GetEGA(color);
	if (PaletteParam.paletteIndex < 8)
		return GetGameBoy(color);
	if (PaletteParam.paletteIndex < 9)
		return GetPico8(color);
	if (PaletteParam.paletteIndex < 10)
		return GetC64(color);
	
}
