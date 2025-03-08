
VERTEX_MAIN_BEGIN
	
	vec3 viewPos = mul(mat3(view), inPos);
	outPos = mul(proj, vec4(viewPos, 1.0)).xyww;

	outWorldPos = inPos;
	
VERTEX_MAIN_END
 