#include "imcodepane.h"

// monospace ImFont for the editor demo, created in App::m_initImgui (app.cpp)
ImFont* GImCodeDemoMonoFont = nullptr;

namespace Panes {

bool ImCodePane::init() {
    const std::string code = R"(
@FRAMEBUFFER

//FORMAT(float or byte)
//COUNT(1 to 8)
//WRAP(clamp or repeat or mirror)
//FILTER(linear or nearest)
//MIPMAP(false or true)
//SIZE(800,600 or picture:file.jpeg)
//RATIO(1.5 or picture:file.jpeg)\n\n";

@UNIFORMS

uniform float(time) iTime;

@VERTEX

layout(location = 0) in vec2 a_position; // Current vertex position

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
}

@NOTE

for show the sapce3d grid, go in menu Settings/show 3d space

@UNIFORMS

uniform(Shape) vec3(-5:5:-3,0.75,0) coefs;
uniform(Shape) vec3(-5:5:0) offset;
uniform(Shape) float(-5:5:3.5) radius;
uniform(Shape) float(0:2:1.23) m;
uniform(Shape) vec3(0:5:1) kp;
uniform(Shape) float(0:1:0.0) kc;
uniform(Shape) int(0:10:10) fractal_iter;
uniform(Shape) vec3(0:5:1) a;
uniform(Shape) vec3(checkbox:false,true,false) pXYZ;
uniform(Shape) vec3(0:20:1) box;

uniform(Diffuse:0) float(0:10:2.0) diffPower;

uniform(Ambiant Occlusion:0) bool(checkbox:true) useAo;
uniform(Ambiant Occlusion:1:useAo==true) float(0:1:0.5) aoRatio;
uniform(Ambiant Occlusion:2:useAo==true) float(0:40:1) aocCoef;

uniform(Orbit trap:0) bool(checkbox:true) useOrbitTrap;
uniform(Orbit trap:1:useOrbitTrap==true) float(0:1:0.5) orbitTrapRatio;
uniform(Orbit trap:2:useOrbitTrap==true) float(0:5:1.0) trapScale;
uniform(Orbit trap:3) vec3(color:0.6,0.2,0.6) colorA;
uniform(Orbit trap:4) vec3(color:0.2,0.6,0.2) colorB;

uniform(Shadow:0) bool(checkbox:false) useShadow;
uniform(Shadow:1) float(0:1:0.5) shaRatio;
uniform(Shadow:2:useShadow==true) int(1:100:20) shaIters;
uniform(Shadow:2:useShadow==true) float(0:1:0.01) shaMinDist;
uniform(Shadow:2:useShadow==true) float(10:300:150) shaMaxDist;
uniform(Shadow:2:useShadow==true) float(0:20:5) shaCoef;

uniform(Specular:0) bool(checkbox:true) useSpecular;
uniform(Specular:1) float(0:20:0.5) speRatio;            
               
uniform(Light) mat4(gizmo:icon=light) lightPos;

uniform(matcap:0) float(checkbox:false) useMatCap;	// use mat cap lighting 
uniform(matcap:1:useMatCap==true) sampler2D(picture:choice=assets/matcap/JG_Gold.png) uTopMatcap;
uniform(matcap:2:useMatCap==true) sampler2D(picture:choice=assets/matcap/material8.png) uBottomMatcap;
uniform(matcap:3:useMatCap==true) float(0:1:1) matCapRatio; // base color ration
uniform(matcap:4:useMatCap==true) float(0:10:1.5) matCapSubstanceStrength; // substance strength for mix 
uniform(matcap:5:useMatCap==true) float(checkbox:false) matCapInverseSubstances; // substance inversion

@FRAGMENT

layout(location = 0) out vec4 fragColor;

#include "space3d.glsl"

float orbitTrapMin; // closest orbit approach to the trap, written by de()
	
float de( vec3 p ) {
	float d = 0.;
	float w = 1.0;
	float k = 0.0;
	orbitTrapMin = 1e20;
	for(int i = 0; i < fractal_iter; i++) {
		p = mod(p - 1.0, 2.0) - 1.0;
		orbitTrapMin = min(orbitTrapMin, dot(p, p)); // distance to origin in folded space
		k = m / dot(p, p);
		k = sqrt(k*k+kc);
		p *= k;
		w *= k;
	}
	float pp = k;
	if (pXYZ.x > 0.5) pp *= p.x;
	if (pXYZ.y > 0.5) pp *= p.y;
	if (pXYZ.z > 0.5) pp *= p.z;
    return abs(pp / w) * 0.125;
}

float sdf(vec3 p) {
	//float op = length(vec2(length(p.xy)-box.y, p.z)-box.x);	op *= op;
	float op = dot(p, p);
	float scale = radius * radius / op;
	p *= scale * kp;
    return de(p + coefs) * m / scale;
}

vec3 nor( in vec3 p, float prec ) {
	vec3 eps = vec3( prec, 0., 0. );
	vec3 nor = vec3(
	    sdf(p+eps.xyy) - sdf(p-eps.xyy),
	    sdf(p+eps.yxy) - sdf(p-eps.yxy),
	    sdf(p+eps.yyx) - sdf(p-eps.yyx) );
	return normalize(nor);
}

float getAmbiantOcclusion(vec3 p, vec3 n, float k) {
    const float aoStep = 0.1; 
	float occl = 0.;
    for(int i = 0; i < 6; ++i)
    {
        float diff = float(i) * aoStep;
        float d = sdf(p + n*diff);
        occl += (diff - d) * pow(2., float(-i));
    }
    return min(1., 1. - k*occl);
}

float getShadow(vec3 ro, vec3 rd, float minD, float maxD, float k) {
    float res = 1.0;
    float d = minD;
	float s = 0.;
    for(int i = 0; i < shaIters; ++i)
    {
        s = sdf(ro + rd * d);
        if( abs(s)<d*d*1e-5 ) return 0.0;
        res = min( res, k * s / d );
		d += s;
        if(d >= maxD) break;
    }
    return res;
}

vec2 getMatCap(vec3 pos, vec3 nor) {
	mat4 modelViewMatrix = _view * _model;
	mat4 normalMatrix = transpose(inverse(modelViewMatrix)); // normalMatrix
	vec4 pp = modelViewMatrix * vec4(pos, 1. );
	vec4 nn = normalMatrix * vec4(nor, 0.);
	vec3 rd = normalize( pp.xyz );
	vec3 n = normalize( nn.xyz );
	vec3 r = reflect(rd, n);
	float m = 2. * sqrt(
		pow( r.x, 2. ) +
		pow( r.y, 2. ) +
		pow( r.z + 1., 2. )
	);
	return r.xy / m + .5;
}

void main(void) {	
	vec3 col = vec3(0);			
	vec3 ro = getRayOrigin();
	vec3 rd = getRayDirection();
    vec3 p = ro;
    float s = 1., d = 0.;
	float prec = 0.001;
    const float md = 1000.;
	for (int i = 0; i < 500; ++i) {
        p = ro + rd * d;
		if (abs(s) < prec) break;
		if (d > md) break;
		s = sdf(p);
		d += s;
	}
    
	if (d < md) {
		CalcDepth(rd, d);	
		vec3 n = nor(p, prec);
		vec3 refl = reflect(rd,n);	
		vec3 ld = normalize(lightPos[3].xyz - p);
						
		float diff = pow(dot(n, ld) * .5 + .5, diffPower);
		float spe = 0.0;
		float fre = pow(clamp(1.0 + dot(n,rd),0.0,1.0), 4.0);
		float ao = 0.0;
		float sha = 0.0;
		
		col += diff;
		
		if (useShadow == true) {
			sha = clamp(getShadow(p, ld, shaMinDist, shaMaxDist, shaCoef), 0. ,1.0);
			col *= sha * shaRatio;
		}
		if (useAo == true) {
			ao = clamp(getAmbiantOcclusion(p, n, aocCoef), 0.0, 1.0);
			col += diff * ao * aoRatio;
		}
		if (useOrbitTrap == true) {
			sdf(p);
			float trapRatio = 1.0 - clamp(orbitTrapMin * trapScale, 0.0, 1.0);
			//float trapRatio = 1.0 - exp(-orbitTrapMin * trapScale);
			col += diff * mix(colorA, colorB, trapRatio) * orbitTrapRatio;
		}
		if (useSpecular == true) {
			spe = pow(max(dot(-rd, reflect(-ld, n)), 0.0), 32.0);
			col += spe * speRatio;	
		}		
		if (useMatCap > 0.5) {
			vec2 tn = getMatCap(p, n);
			vec4 bottom = (matCapInverseSubstances > 0.5) ? texture( uBottomMatcap, tn) : texture(uTopMatcap, tn);
			vec4 top = (matCapInverseSubstances > 0.5) ? texture(uTopMatcap, tn) : texture(uBottomMatcap, tn);
			col += clamp(mix(bottom.rgb, top.rgb, vec3(matCapSubstanceStrength)) * matCapRatio, 0.0, 1.0);
		}

		col = clamp(col, 0., 1.);
	} else {
		discard;
	}	
	
	fragColor = vec4(col,1);
}

)";
    m_editor.init();
    m_editor.getStyle().font = GImCodeDemoMonoFont;
    m_editor.setLanguage("glsl");
    m_editor.setMarkers({{3, IM_COL32(220, 80, 80, 255), "breakpoint", 0}});
    m_editor.setDecorations({{{{4, 8}, {4, 11}}, im::Code::DecoKind::Squiggle, IM_COL32(90, 170, 255, 255), "the loop index"}});
    m_editor.setDiagnostics({{{{1, 0}, {1, 17}}, im::Code::Severity::Warning, "unused include"}});
    m_editor.setText(code.data(), (uint64_t)code.size());
    return true;
}

void ImCodePane::unit() {
}

bool ImCodePane::drawPanes(bool* apOpened, LayoutPaneUserDatas apUserDatas) {
    bool change = false;
    if (apOpened != nullptr && *apOpened) {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
        if (ImGui::Begin(getName().c_str(), apOpened, flags)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
#endif
            m_editor.Render("##imcode_editor", ImGui::GetContentRegionAvail());
        }
        ImGui::End();
    }
    return change;
}

ez::xml::Nodes ImCodePane::getXmlNodes(const std::string& aUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

bool ImCodePane::setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) {
    return false;  // leaf dont explore childs
}

}  // namespace pane
