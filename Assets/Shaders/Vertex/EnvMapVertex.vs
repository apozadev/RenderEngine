
//#extension GL_EXT_multiview : require

//CBuffer(ViewProjs, 2)
//{
//  mat4[6] aViewProjs;
//};

VERTEX_MAIN_BEGIN
	
	outPos = vec4(inPos, 1);//mul(aViewProjs[gl_ViewIndex], vec4(inPos, 1.0));
	outWorldPos = inPos;
	
VERTEX_MAIN_END
 