#include "collision.h"

namespace undicht {

    namespace physics {

		/*template<typename T0, typename T1>
        Collision<T0, T1>::Collision(T0* object0, T1* object1, bool will_collide, double time, const vec3i& position, const vec3f& normal0, const vec3f& normal1) 
        : _object0(object0), _object1(object1), _will_collide(will_collide), _time(time), _position(position), _normal0(normal0), _normal1(normal1) {

        }

		template<typename T0, typename T1>
		void Collision<T0, T1>::setObject0(T0* object0) {

            _object0 = object0;
        }

		template<typename T0, typename T1>
        void Collision<T0, T1>::setObject1(T1* object1) {

            _object1 = object1;
        }

		template<typename T0, typename T1>
		void Collision<T0, T1>::setWillCollide(bool will_collide) {

            _will_collide = will_collide;
        }

		template<typename T0, typename T1>
		void Collision<T0, T1>::setTime(double time) {

            _time = time;
        }

		template<typename T0, typename T1>
        void Collision<T0, T1>::setPosition(const vec3i& position) {

            _position = position;
        }

		template<typename T0, typename T1>
        void Collision<T0, T1>::setNormal0(const vec3f& normal0) {

            _normal0 = normal0;
        }

        template<typename T0, typename T1>
        void Collision<T0, T1>::setNormal1(const vec3f& normal1) {

            _normal1 = normal1;
        }

		template<typename T0, typename T1>
		T0* Collision<T0, T1>::getObject0() const {

            return _object0;
        }

		template<typename T0, typename T1>
        T1* Collision<T0, T1>::getObject1() const {

            return _object1;
        }

		template<typename T0, typename T1>
		bool Collision<T0, T1>::willCollide() const {

            return _will_collide;
        }

		template<typename T0, typename T1>
		double Collision<T0, T1>::getTime() const {

            return _time;
        }

		template<typename T0, typename T1>
        const vec3i& Collision<T0, T1>::getPosition() const {

            return _position;
        }

		template<typename T0, typename T1>
        const vec3f& Collision<T0, T1>::getNormal0() const {

            return _normal0;
        }

		template<typename T0, typename T1>
        const vec3f& Collision<T0, T1>::getNormal1() const {

            return _normal1;
        }*/
        
        // implementation of collisions between objects

        class SphereObject;
        template struct Collision<SphereObject, SphereObject>;

    }

} // undicht