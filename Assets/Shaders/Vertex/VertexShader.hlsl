
VERTEX_MAIN_BEGIN

	vec4 worldPos = mul(modelMat, vec4(inPos, 1.0));
	outPos = mul(viewProj, worldPos);
	outWorldPos = worldPos.xyz;
	outNormal = normalize(mul(normalMat, vec4(inNormal, 0.0))).xyz;
	outUv = inUv;
	
VERTEX_MAIN_END 