#include "SObjSaver.h"

#include "SL_Common.h"

using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace tinyxml2;
using namespace std;

SObjSaver::SObjSaver() {
	RegMemberFunc<SObj>(&SObjSaver::Visit);

	RegMemberFunc<CmptCamera>(&SObjSaver::Visit);

	RegMemberFunc<CmptGeometry>(&SObjSaver::Visit);
	RegMemberFunc<Sphere>(&SObjSaver::Visit);
	RegMemberFunc<Plane>(&SObjSaver::Visit);
	RegMemberFunc<TriMesh>(&SObjSaver::Visit);

	RegMemberFunc<CmptLight>(&SObjSaver::Visit);
	RegMemberFunc<AreaLight>(&SObjSaver::Visit);
	RegMemberFunc<PointLight>(&SObjSaver::Visit);

	RegMemberFunc<CmptMaterial>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_CookTorrance>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_Diffuse>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_Emission>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_Glass>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_MetalWorkflow>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_Mirror>(&SObjSaver::Visit);
	RegMemberFunc<BSDF_FrostedGlass>(&SObjSaver::Visit);

	RegMemberFunc<CmptTransform>(&SObjSaver::Visit);
}

void SObjSaver::Init(const string & path) {
	this->path = path;
	doc.Clear();
	parentEleStack.clear();
}

void SObjSaver::Member(XMLElement * parent, const function<void()> & func) {
	parentEleStack.push_back(parent);
	func();
	parentEleStack.pop_back();
}

void SObjSaver::NewEle(const char * const name, CPtr<Image> img) {
	Member(NewEle(name), [=]() {
		Visit(img);
	});
}

void SObjSaver::Visit(CPtr<Image> img) {
	if (!img || !img->IsValid() || img->GetPath().empty())
		return;

	NewEle(str::Image::path, img->GetPath());
}

void SObjSaver::Visit(Ptr<Image> img) {
	Visit(CPtr<Image>(img));
}

void SObjSaver::Visit(Ptr<SObj> sobj) {
	// sobj
	auto ele = doc.NewElement(str::SObj::type);
	if (parentEleStack.empty())
		doc.InsertEndChild(ele);
	else
		parentEleStack.back()->InsertEndChild(ele);

	Member(ele, [=](){
		// name
		NewEle(str::SObj::name, sobj->name);

		// components
		NewEle(str::SObj::components, [=]() {
			for (auto component : sobj->GetAllComponents())
				component->Accept(This());
		});

		// children
		NewEle(str::SObj::children, [=]() {
			for (auto child : sobj->GetChildren())
				child->Accept(This());
		});
	});

	// save
	if (parentEleStack.empty())
		doc.SaveFile(path.c_str());
}

void SObjSaver::Visit(Ptr<CmptCamera> cmpt) {
	NewEle(str::CmptCamera::type, [=]() {
		NewEle(str::CmptCamera::fov, cmpt->GetFOV());
		NewEle(str::CmptCamera::ar, cmpt->GetAspectRatio());
		NewEle(str::CmptCamera::nearPlane, cmpt->nearPlane);
		NewEle(str::CmptCamera::farPlane, cmpt->farPlane);
	});
}

void SObjSaver::Visit(Ptr<CmptGeometry> cmpt) {
	NewEle(str::CmptGeometry::type, [=]() {
		NewEle(str::CmptGeometry::primitive, [=]() {
			if (cmpt->primitive)
				cmpt->primitive->Accept(This());
		});
	});
}

void SObjSaver::Visit(Ptr<Sphere> sphere) {
	NewEle(str::Sphere::type);
}

void SObjSaver::Visit(Ptr<Plane> plane) {
	NewEle(str::Plane::type);
}

void SObjSaver::Visit(Ptr<TriMesh> mesh) {
	NewEle(str::TriMesh::type, [=]() {
		if (mesh->GetType() == TriMesh::ENUM_TYPE::INVALID)
			return;

		map<TriMesh::ENUM_TYPE, function<void()>> type2func;
		type2func[TriMesh::ENUM_TYPE::INVALID] = [=]() {
			// do nothing
		};
		type2func[TriMesh::ENUM_TYPE::CODE] = [=]() {
			// not support
		};

		type2func[TriMesh::ENUM_TYPE::CUBE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::CUBE);
		};
		type2func[TriMesh::ENUM_TYPE::SPHERE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::SPHERE);
		};
		type2func[TriMesh::ENUM_TYPE::PLANE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::PLANE);
		};
		type2func[TriMesh::ENUM_TYPE::FILE] = [=]() {
			// not support
		};

		auto target = type2func.find(mesh->GetType());
		if (target == type2func.end())
			return;

		target->second();
	});
}

void SObjSaver::Visit(Ptr<CmptLight> cmpt) {
	NewEle(str::CmptLight::type, [=]() {
		NewEle(str::CmptLight::light, [=]() {
			if (cmpt->light)
				cmpt->light->Accept(This());
		});
	});
}

void SObjSaver::Visit(Ptr<AreaLight> areaLight) {
	NewEle(str::AreaLight::type, [=]() {
		NewEle(str::AreaLight::color, areaLight->color);
		NewEle(str::AreaLight::intensity, areaLight->intensity);
		NewEle(str::AreaLight::width, areaLight->width);
		NewEle(str::AreaLight::height, areaLight->height);
	});
}

void SObjSaver::Visit(Ptr<PointLight> pointLight) {
	NewEle(str::PointLight::type, [=]() {
		NewEle(str::PointLight::color, pointLight->color);
		NewEle(str::PointLight::intensity, pointLight->intensity);
		NewEle(str::PointLight::linear, pointLight->linear);
		NewEle(str::PointLight::quadratic, pointLight->quadratic);
	});
}

void SObjSaver::Visit(Ptr<CmptMaterial> cmpt) {
	NewEle(str::CmptMaterial::type, [=]() {
		NewEle(str::CmptMaterial::material, [=]() {
			if (cmpt->material)
				cmpt->material->Accept(This());
		});
	});
}

void SObjSaver::Visit(Ptr<BSDF_CookTorrance> bsdf){
	NewEle(str::BSDF_CookTorrance::type, [=]() {
		NewEle(str::BSDF_CookTorrance::ior, bsdf->ior);
		NewEle(str::BSDF_CookTorrance::roughness, bsdf->m);
		NewEle(str::BSDF_CookTorrance::refletance, bsdf->refletance);
		NewEle(str::BSDF_CookTorrance::albedo, bsdf->albedo);
	});
}

void SObjSaver::Visit(Ptr<BSDF_Diffuse> bsdf){
	NewEle(str::BSDF_Diffuse::type, [=]() {
		NewEle(str::BSDF_Diffuse::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_Diffuse::albedoTexture, [=]() {
			Visit(bsdf->albedoTexture);
		});
	});
}

void SObjSaver::Visit(Ptr<BSDF_Emission> bsdf){
	NewEle(str::BSDF_Emission::type, [=]() {
		NewEle(str::BSDF_Emission::color, bsdf->color);
		NewEle(str::BSDF_Emission::intensity, bsdf->intensity);
	});
}

void SObjSaver::Visit(Ptr<BSDF_Glass> bsdf){
	NewEle(str::BSDF_Glass::type, [=]() {
		NewEle(str::BSDF_Glass::ior, bsdf->ior);
		NewEle(str::BSDF_Glass::transmittance, bsdf->transmittance);
		NewEle(str::BSDF_Glass::reflectance, bsdf->reflectance);
	});
}

void SObjSaver::Visit(Ptr<BSDF_MetalWorkflow> bsdf){
	NewEle(str::BSDF_MetalWorkflow::type, [=]() {
		NewEle(str::BSDF_MetalWorkflow::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_MetalWorkflow::albedoTexture, [=]() {
			Visit(bsdf->GetAlbedoTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::metallicFactor, bsdf->metallicFactor);
		NewEle(str::BSDF_MetalWorkflow::metallicTexture, [=]() {
			Visit(bsdf->GetMetallicTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::roughnessFactor, bsdf->roughnessFactor);
		NewEle(str::BSDF_MetalWorkflow::roughnessTexture, [=]() {
			Visit(bsdf->GetRoughnessTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::aoTexture, [=]() {
			Visit(bsdf->GetAOTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::normalTexture, [=]() {
			Visit(bsdf->GetNormalTexture());
		});
	});
}

void SObjSaver::Visit(Ptr<BSDF_FrostedGlass> bsdf) {
	NewEle(str::BSDF_FrostedGlass::type, [=]() {
		NewEle(str::BSDF_FrostedGlass::IOR, bsdf->ior);

		NewEle(str::BSDF_FrostedGlass::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_FrostedGlass::colorTexture, bsdf->colorTexture);

		NewEle(str::BSDF_FrostedGlass::roughnessFactor, bsdf->roughnessFactor);
		NewEle(str::BSDF_FrostedGlass::roughnessTexture, bsdf->roughnessTexture);

		NewEle(str::BSDF_FrostedGlass::aoTexture, bsdf->aoTexture);

		NewEle(str::BSDF_FrostedGlass::normalTexture, bsdf->normalTexture);
	});
}

void SObjSaver::Visit(Ptr<BSDF_Mirror> bsdf){
	NewEle(str::BSDF_Mirror::type, [=]() {
		NewEle(str::BSDF_Mirror::reflectance, bsdf->reflectance);
	});
}

void SObjSaver::Visit(Ptr<CmptTransform> cmpt) {
	NewEle(str::CmptTransform::type, [=]() {
		NewEle(str::CmptTransform::Position, cmpt->GetPosition());
		NewEle(str::CmptTransform::Rotation, cmpt->GetRotation());
		NewEle(str::CmptTransform::Scale, cmpt->GetScale());
	});
}
