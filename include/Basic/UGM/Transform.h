#ifndef _CPPUTIL_BASIC_MATH_UGM_TRANSFORM_H_
#define _CPPUTIL_BASIC_MATH_UGM_TRANSFORM_H_

#include <Basic/UGM/Point3.h>
#include <Basic/UGM/Vector3.h>
#include <Basic/UGM/Normal.h>
#include <Basic/UGM/Quat.h>
#include <Basic/UGM/EulerYXZ.h>
#include <Basic/UGM/Mat4x4.h>
#include <Basic/UGM/BBox.h>
#include <Basic/UGM/Ray.h>
#include <Basic/UGM/Scale.h>

namespace CppUtil {
	namespace Basic {

		// ����ϵ
		class Transform {
		public:
			Transform(float d) : m(d), mInv(1.f/d) { }
			Transform() :Transform(1.f) { }
			Transform(const float mat[4][4]) : m(Mat4f(mat)), mInv(m.Inverse()) { }
			Transform(const Mat4f & m) : m(m), mInv(m.Inverse()) { }
			Transform(const Mat4f & m, const Mat4f & mInv) : m(m), mInv(mInv) {}
			Transform(const Point3 & pos, const Scalef & scale = Vec3f(1.0f), const Quatf & rot = Quatf()) { Init(pos, scale, rot); }
			Transform(const Point3 & pos, const Scalef & scale, const Vec3 & axis, float theta) { Init(pos, scale, axis, theta); }
			Transform(const Point3 & pos, const Scalef & scale, const EulerYXZf & euler) { Init(pos, scale, euler); }

		public:
			const Mat4f & GetMatrix() const { return m; }
			const Mat4f & GetInverseMatrix() const { return mInv; }

		public:
			void Init(const Point3 & pos, const Scalef & scale = Vec3f(1.0f), const Quatf & rot = Quatf());
			void Init(const Point3 & pos, const Scalef & scale, const Vec3 & axis, float theta) {
				Init(pos, scale, Quatf(axis, theta));
			}
			void Init(const Point3 & pos, const Scalef & scale, const EulerYXZf & euler) {
				Init(pos, scale, euler.ToQuat());
			}

			const Transform Inverse() const {
				return Transform(mInv, m);
			}

			const Transform Transpose() const {
				return Transform(m.Transpose(), mInv.Transpose());
			}

			struct PosRotScale {
				Point3 pos;
				Quatf rot;
				Scalef scale;
			};
			const PosRotScale Decompose() const;

			const Point3 Position() const {
				return m.GetCol(3);
			}
			const Quatf RotationQuat() const {
				return Decompose().rot;
			}
			const EulerYXZf RotationEulerYXZ() const;
			const Scalef Scale() const {
				return Scalef(m.GetCol(0).Norm(), m.GetCol(1).Norm(), m.GetCol(2).Norm());
			}

			bool operator==(const Transform & rhs)const {
				return m == rhs.m && mInv == rhs.mInv;
			}

			bool operator!=(const Transform & rhs)const {
				return m != rhs.m || mInv != rhs.mInv;
			}

		public:
			static const Transform Translate(float x, float y, float z);
			static const Transform Translate(const Vec3 & delta) {
				return Translate(delta.x, delta.y, delta.z);
			}
			static const Transform Scale(float x, float y, float z);
			static const Transform Scale(const Scalef & scale) {
				return Scale(scale.x, scale.y, scale.z);
			}
			static const Transform RotateX(float theta);
			static const Transform RotateY(float theta);
			static const Transform RotateZ(float theta);

			// first Y, then X, final Z
			static const Transform Rotate(const EulerYXZf & euler) {
				return RotateZ(euler.z) * RotateX(euler.x) * RotateY(euler.y);
			}
			
			static const Transform Rotate(const Vec3 &axis, float theta);
			static const Transform Rotate(const Quatf & q);

			/*
			����������ת�����������

			@param
				pos: ���λ��
				target: �۲�Ŀ���λ��
				up: �Ϸ���
			*/
			static const Transform LookAt(const Point3 &pos, const Point3 &target, const Vec3 &up = Vec3(0, 1, 0));


			/*
			�����任�����������ת������׼����ϵ [-1, 1]^3
			*/
			static const Transform Orthographic(float width, float height, float zNear, float zFar);

			/*
			͸�ӱ任�����������ת������׼����ϵ [-1, 1]^3
			
			@param
				fovy: �����򣬽Ƕ�
				aspect: ���߱�
				zNear: ��ƽ�� z
				zFar: Զƽ�� z
			*/
			static const Transform Perspcetive(float fovy, float aspect, float zNear, float zFar);

		public:
			const Point3 operator()(const Point3 & p) const;
			const Vec3 operator()(const Vec3 & v) const;
			const Normalf operator()(const Normalf & n) const;
			const BBoxf operator()(const BBoxf & box) const;
			const Ray operator()(const Ray & ray) const;

			Point3 & ApplyTo(Point3 & p) const;
			Vec3 & ApplyTo(Vec3 & v) const;
			Normalf & ApplyTo(Normalf & n) const;
			BBoxf & ApplyTo(BBoxf & box) const;
			Ray & ApplyTo(Ray & ray) const;

			const Transform operator*(const Transform & rhs) const { return Transform(m * rhs.m, rhs.mInv * mInv); }
			Transform & operator*=(const Transform & rhs) {
				m *= rhs.m;
				mInv = rhs.mInv * mInv;
				return *this;
			}

		private:
			Mat4f m;
			Mat4f mInv;
		};
	}

	using Transform = Basic::Transform;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_TRANSFORM_H_