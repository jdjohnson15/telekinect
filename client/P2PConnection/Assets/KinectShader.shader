// Shader created with Shader Forge v1.16 
// Shader Forge (c) Neat Corporation / Joachim Holmer - http://www.acegikmo.com/shaderforge/
// Note: Manually altering this data may prevent you from opening it in Shader Forge
/*SF_DATA;ver:1.16;sub:START;pass:START;ps:flbk:,iptp:0,cusa:False,bamd:0,lico:1,lgpr:1,limd:0,spmd:1,trmd:0,grmd:0,uamb:True,mssp:True,bkdf:False,hqlp:False,rprd:False,enco:False,rmgx:True,rpth:0,hqsc:True,nrmq:1,nrsp:0,vomd:0,spxs:False,tesm:0,culm:0,bsrc:0,bdst:1,dpts:2,wrdp:True,dith:0,rfrpo:True,rfrpn:Refraction,ufog:False,aust:True,igpj:False,qofs:0,qpre:1,rntp:1,fgom:False,fgoc:False,fgod:False,fgor:False,fgmd:0,fgcr:0.5,fgcg:0.5,fgcb:0.5,fgca:1,fgde:0.01,fgrn:0,fgrf:300,ofsf:0,ofsu:0,f2p0:False;n:type:ShaderForge.SFN_Final,id:3138,x:33053,y:32485,varname:node_3138,prsc:2|emission-7886-RGB,voffset-6337-OUT;n:type:ShaderForge.SFN_Tex2d,id:3076,x:31207,y:32677,ptovrint:False,ptlb:depth,ptin:_depth,varname:node_3076,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,ntxv:0,isnm:False;n:type:ShaderForge.SFN_Vector4Property,id:530,x:32307,y:32791,ptovrint:False,ptlb:node_530,ptin:_node_530,varname:node_530,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,v1:0,v2:0,v3:1,v4:0;n:type:ShaderForge.SFN_Multiply,id:6337,x:32703,y:32943,varname:node_6337,prsc:2|A-530-XYZ,B-9029-OUT;n:type:ShaderForge.SFN_Tex2d,id:7886,x:32674,y:32529,ptovrint:False,ptlb:rgb,ptin:_rgb,varname:node_7886,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,ntxv:0,isnm:False;n:type:ShaderForge.SFN_Add,id:2520,x:31948,y:33024,varname:node_2520,prsc:2|A-3153-OUT,B-9961-OUT;n:type:ShaderForge.SFN_ValueProperty,id:9961,x:31630,y:33235,ptovrint:False,ptlb:add,ptin:_add,varname:node_9961,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,v1:5;n:type:ShaderForge.SFN_Divide,id:9029,x:32307,y:33036,varname:node_9029,prsc:2|A-2520-OUT,B-3584-OUT;n:type:ShaderForge.SFN_ValueProperty,id:3584,x:32083,y:33242,ptovrint:False,ptlb:divide,ptin:_divide,varname:node_3584,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,v1:5;n:type:ShaderForge.SFN_Multiply,id:4214,x:32854,y:32349,varname:node_4214,prsc:2|A-7886-RGB,B-387-OUT;n:type:ShaderForge.SFN_Vector3,id:387,x:32640,y:32332,varname:node_387,prsc:2,v1:0,v2:0,v3:2;n:type:ShaderForge.SFN_OneMinus,id:1691,x:32921,y:32995,varname:node_1691,prsc:2|IN-6337-OUT;n:type:ShaderForge.SFN_OneMinus,id:3210,x:31806,y:32834,varname:node_3210,prsc:2|IN-3153-OUT;n:type:ShaderForge.SFN_If,id:3153,x:31617,y:32732,varname:node_3153,prsc:2|A-3076-A,B-4799-OUT,GT-3076-RGB,EQ-9927-OUT,LT-9927-OUT;n:type:ShaderForge.SFN_Add,id:9927,x:31419,y:32579,varname:node_9927,prsc:2|A-9308-OUT,B-3076-RGB;n:type:ShaderForge.SFN_Vector3,id:2109,x:31003,y:32566,varname:node_2109,prsc:2,v1:1,v2:1,v3:1;n:type:ShaderForge.SFN_Multiply,id:9308,x:31252,y:32448,varname:node_9308,prsc:2|A-2109-OUT,B-3135-OUT;n:type:ShaderForge.SFN_ValueProperty,id:3135,x:31036,y:32499,ptovrint:False,ptlb:setZeroAlphaValue,ptin:_setZeroAlphaValue,varname:node_3135,prsc:2,glob:False,taghide:False,taghdr:False,tagprd:False,tagnsco:False,tagnrm:False,v1:1;n:type:ShaderForge.SFN_Vector1,id:4799,x:31217,y:33040,varname:node_4799,prsc:2,v1:0;proporder:3076-530-7886-9961-3584-3135;pass:END;sub:END;*/

Shader "KinectShader" {
    Properties {
        _depth ("depth", 2D) = "white" {}
        _node_530 ("node_530", Vector) = (0,0,1,0)
        _rgb ("rgb", 2D) = "white" {}
        _add ("add", Float ) = 5
        _divide ("divide", Float ) = 5
        _setZeroAlphaValue ("setZeroAlphaValue", Float ) = 1
    }
    SubShader {
        Tags {
            "RenderType"="Opaque"
        }
        Pass {
            Name "FORWARD"
            Tags {
                "LightMode"="ForwardBase"
            }
            
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_FORWARDBASE
            #include "UnityCG.cginc"
            #pragma multi_compile_fwdbase_fullshadows
            #pragma exclude_renderers gles3 metal d3d11_9x xbox360 xboxone ps3 ps4 psp2 
            #pragma target 3.0
            #pragma glsl
            uniform sampler2D _depth; uniform float4 _depth_ST;
            uniform float4 _node_530;
            uniform sampler2D _rgb; uniform float4 _rgb_ST;
            uniform float _add;
            uniform float _divide;
            uniform float _setZeroAlphaValue;
            struct VertexInput {
                float4 vertex : POSITION;
                float2 texcoord0 : TEXCOORD0;
            };
            struct VertexOutput {
                float4 pos : SV_POSITION;
                float2 uv0 : TEXCOORD0;
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv0 = v.texcoord0;
                float4 _depth_var = tex2Dlod(_depth,float4(TRANSFORM_TEX(o.uv0, _depth),0.0,0));
                float node_3153_if_leA = step(_depth_var.a,0.0);
                float node_3153_if_leB = step(0.0,_depth_var.a);
                float3 node_9927 = ((float3(1,1,1)*_setZeroAlphaValue)+_depth_var.rgb);
                float3 node_3153 = lerp((node_3153_if_leA*node_9927)+(node_3153_if_leB*_depth_var.rgb),node_9927,node_3153_if_leA*node_3153_if_leB);
                float3 node_6337 = (_node_530.rgb*((node_3153+_add)/_divide));
                v.vertex.xyz += node_6337;
                o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
                return o;
            }
            float4 frag(VertexOutput i) : COLOR {
/////// Vectors:
////// Lighting:
////// Emissive:
                float4 _rgb_var = tex2D(_rgb,TRANSFORM_TEX(i.uv0, _rgb));
                float3 emissive = _rgb_var.rgb;
                float3 finalColor = emissive;
                return fixed4(finalColor,1);
            }
            ENDCG
        }
        Pass {
            Name "ShadowCaster"
            Tags {
                "LightMode"="ShadowCaster"
            }
            Offset 1, 1
            
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #define UNITY_PASS_SHADOWCASTER
            #include "UnityCG.cginc"
            #include "Lighting.cginc"
            #pragma fragmentoption ARB_precision_hint_fastest
            #pragma multi_compile_shadowcaster
            #pragma exclude_renderers gles3 metal d3d11_9x xbox360 xboxone ps3 ps4 psp2 
            #pragma target 3.0
            #pragma glsl
            uniform sampler2D _depth; uniform float4 _depth_ST;
            uniform float4 _node_530;
            uniform float _add;
            uniform float _divide;
            uniform float _setZeroAlphaValue;
            struct VertexInput {
                float4 vertex : POSITION;
                float2 texcoord0 : TEXCOORD0;
            };
            struct VertexOutput {
                V2F_SHADOW_CASTER;
                float2 uv0 : TEXCOORD1;
            };
            VertexOutput vert (VertexInput v) {
                VertexOutput o = (VertexOutput)0;
                o.uv0 = v.texcoord0;
                float4 _depth_var = tex2Dlod(_depth,float4(TRANSFORM_TEX(o.uv0, _depth),0.0,0));
                float node_3153_if_leA = step(_depth_var.a,0.0);
                float node_3153_if_leB = step(0.0,_depth_var.a);
                float3 node_9927 = ((float3(1,1,1)*_setZeroAlphaValue)+_depth_var.rgb);
                float3 node_3153 = lerp((node_3153_if_leA*node_9927)+(node_3153_if_leB*_depth_var.rgb),node_9927,node_3153_if_leA*node_3153_if_leB);
                float3 node_6337 = (_node_530.rgb*((node_3153+_add)/_divide));
                v.vertex.xyz += node_6337;
                o.pos = mul(UNITY_MATRIX_MVP, v.vertex);
                TRANSFER_SHADOW_CASTER(o)
                return o;
            }
            float4 frag(VertexOutput i) : COLOR {
/////// Vectors:
                SHADOW_CASTER_FRAGMENT(i)
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
    CustomEditor "ShaderForgeMaterialInspector"
}
