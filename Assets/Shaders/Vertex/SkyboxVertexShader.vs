
VERTEX_MAIN_BEGIN
	
	vec3 viewPos = mul(view, vec4(inPos, 0.0)).xyz;
	outPos = mul(proj, vec4(viewPos, 1.0)).xyww;

	outWorldPos = inPos;
	
VERTEX_MAIN_END
 