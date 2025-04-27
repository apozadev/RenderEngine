
CBuffer(ViewProjs, 2)
{
  mat4 aViewProjs[6];
};

GEOM_MAIN_BEGIN

for (int i = 0; i < 6; ++i) 
{	
	for (int j = 0; j < 3; ++j) 
	{
		outLayer = i;
		outPos = mul(aViewProjs[i], inPos(j));
		outWorldPos = inPos(j).xyz;
		END_VERTEX
	}
	END_PRIMITIVE
}

GEOM_MAIN_END
