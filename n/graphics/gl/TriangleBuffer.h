/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/

#ifndef N_GRAPHICS_GL_TRIANGLEBUFFER
#define N_GRAPHICS_GL_TRIANGLEBUFFER

#include "VertexBuffer.h"
#include <n/defines.h>
#ifndef N_NO_GL

namespace n {
namespace graphics {
namespace gl {

template<typename T>
class VertexArrayObject;

template<typename T = float>
class TriangleBuffer
{
	struct TriData
	{
		typedef const uint * const_iterator;
		TriData(uint a, uint b, uint c) : vts{a, b, c} {
		}

		uint vts[3];

		const_iterator begin() const {
			return vts;
		}

		const_iterator end() const {
			return vts + 3;
		}

	};

	public:
		class FreezedTriangleBuffer
		{
			public:
				FreezedTriangleBuffer(FreezedTriangleBuffer &&fr) {
					indexes.swap(fr.indexes);
					vertices.swap(fr.vertices);
				}

				FreezedTriangleBuffer(const FreezedTriangleBuffer &) = default;

				core::Array<uint> indexes;
				core::Array<Vertex<T>> vertices;
				T radius;

			private:
				friend class TriangleBuffer;
				FreezedTriangleBuffer(const TriangleBuffer &b) : indexes(b.trianglesData), vertices(b.vertices.getVertices()), radius(0) {
					for(const TriData &t : b.trianglesData) {
						math::Vec<3, T> edges[] = {vertices[t.vts[1]].p() - vertices[t.vts[0]].p(), vertices[t.vts[2]].p() - vertices[t.vts[0]].p()};
						T dt[] = {vertices[t.vts[1]].t().y() - vertices[t.vts[0]].t().y(), vertices[t.vts[2]].t().y() - vertices[t.vts[0]].t().y()};
						T db[] = {vertices[t.vts[1]].t().x() - vertices[t.vts[0]].t().x(), vertices[t.vts[2]].t().x() - vertices[t.vts[0]].t().x()};
						T scale = T(1) / ((db[0] * dt[1]) - (db[1] * dt[0]));
						math::Vec<3, T> ta = ((edges[0] * dt[1]) - (edges[1] * dt[0])) * scale;
						vertices[t.vts[0]].t() += ta;
						vertices[t.vts[1]].t() += ta;
						vertices[t.vts[2]].t() += ta;
					}
					if(!vertices.isEmpty() && vertices.first().n().isNull()) {
						for(const TriData &t : b.trianglesData) {
							math::Vec<3, T> nr = (vertices[t.vts[1]].p() - vertices[t.vts[0]].p()).cross(vertices[t.vts[2]].p() - vertices[t.vts[0]].p()).normalized();
							vertices[t.vts[0]].n() += nr;
							vertices[t.vts[1]].n() += nr;
							vertices[t.vts[2]].n() += nr;
						}
					}
					for(Vertex<T> &v : vertices) {
						radius = std::max(v.p().length2(), radius);
						v.t().normalize();
						v.n().normalize();
					}
					radius = sqrt(radius);
				}


		};

		class Triangle
		{
			public:
				const Vertex<T> &operator[](uint i) const {
					return getVertex(i);
				}

				const Vertex<T> &getVertex(uint i) const {
					return buffer->vertices[buffer->trianglesData[index][i]];
				}

			private:
				friend class TriangleBuffer;
				Triangle(uint i, TriangleBuffer<T> *b) : index(i), buffer(b) {
				}

				Vertex<T> &getVertex(uint i) {
					buffer->modified = true;
					return buffer->vertices[buffer->trianglesData[index][i]];
				}

				uint index;
				TriangleBuffer<T> *buffer;
		};

		TriangleBuffer(const VertexBuffer<T> &vb = VertexBuffer<T>()) : vertices(vb) {
		}

		Triangle append(const Vertex<T> &a, const Vertex<T> &b, const Vertex<T> &c) {
			trianglesData.append(TriData(vertices.append(a), vertices.append(b), vertices.append(c)));
			Triangle tr(trianglesData.size() - 1, this);
			triangles.append(tr);
			return tr;
		}

		Triangle append(uint a, uint b, uint c) {
			trianglesData.append(TriData(a, b, c));
			Triangle tr(trianglesData.size() - 1, this);
			triangles.append(tr);
			return tr;
		}

		uint size() const {
			return triangles.size();
		}

		typename core::Array<Triangle>::iterator begin() {
			return triangles.begin();
		}

		typename core::Array<Triangle>::iterator end() {
			return triangles.end();
		}

		typename core::Array<Triangle>::const_iterator begin() const {
			return triangles.begin();
		}

		typename core::Array<Triangle>::const_iterator end() const {
			return triangles.end();
		}

		const core::Array<Triangle> &getTriangles() const {
			return triangles;
		}

		const VertexBuffer<T> &getVertices() const {
			return vertices;
		}

		const FreezedTriangleBuffer freezed() const {
			return FreezedTriangleBuffer(*this);
		}

		static const FreezedTriangleBuffer &getSphere();
		static const FreezedTriangleBuffer &getCube();

	private:
		template<typename U>
		friend class VertexArrayObject;

		core::Array<TriData> trianglesData;
		core::Array<Triangle> triangles;

		VertexBuffer<T> vertices;
};























template<typename T>
const typename TriangleBuffer<T>::FreezedTriangleBuffer &TriangleBuffer<T>::getSphere() {
	static FreezedTriangleBuffer *fr = 0;
	if(!fr) {
		TriangleBuffer<T> tris(VertexBuffer<T>(core::Array<Vertex<T>>(Vertex<T>(math::Vec<3, T>(0, -1, 0)), Vertex<T>(math::Vec<3, T>(0.2031809985637664794921875, -0.96794998645782470703125, 0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(-0.077606998383998870849609375, -0.96794998645782470703125, 0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(0.723607003688812255859375, -0.4472199976444244384765625, 0.52572500705718994140625)), Vertex<T>(math::Vec<3, T>(0.60954701900482177734375, -0.65751898288726806640625, 0.442856013774871826171875)), Vertex<T>(math::Vec<3, T>(0.81272900104522705078125, -0.5023009777069091796875, 0.2952379882335662841796875)), Vertex<T>(math::Vec<3, T>(-0.2511470019817352294921875, -0.967948973178863525390625, 0)), Vertex<T>(math::Vec<3, T>(-0.077606998383998870849609375, -0.96794998645782470703125, -0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(0.2031809985637664794921875, -0.96794998645782470703125, -0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(0.8606979846954345703125, -0.2511509954929351806640625, 0.4428580105304718017578125)), Vertex<T>(math::Vec<3, T>(-0.276387989521026611328125, -0.4472199976444244384765625, 0.85064899921417236328125)), Vertex<T>(math::Vec<3, T>(-0.02963900007307529449462890625, -0.502301990985870361328125, 0.86418402194976806640625)), Vertex<T>(math::Vec<3, T>(-0.1552149951457977294921875, -0.2511520087718963623046875, 0.955421984195709228515625)), Vertex<T>(math::Vec<3, T>(-0.89442598819732666015625, -0.4472160041332244873046875, 0)), Vertex<T>(math::Vec<3, T>(-0.83105099201202392578125, -0.502299010753631591796875, 0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(-0.956625998020172119140625, -0.251148998737335205078125, 0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(-0.276387989521026611328125, -0.4472199976444244384765625, -0.85064899921417236328125)), Vertex<T>(math::Vec<3, T>(-0.48397099971771240234375, -0.502301990985870361328125, -0.71656501293182373046875)), Vertex<T>(math::Vec<3, T>(-0.4360069930553436279296875, -0.2511520087718963623046875, -0.864188015460968017578125)), Vertex<T>(math::Vec<3, T>(0.723607003688812255859375, -0.4472199976444244384765625, -0.52572500705718994140625)), Vertex<T>(math::Vec<3, T>(0.531940996646881103515625, -0.502301990985870361328125, -0.681711971759796142578125)), Vertex<T>(math::Vec<3, T>(0.687159001827239990234375, -0.2511520087718963623046875, -0.6817150115966796875)), Vertex<T>(math::Vec<3, T>(0.687159001827239990234375, -0.2511520087718963623046875, 0.6817150115966796875)), Vertex<T>(math::Vec<3, T>(-0.4360069930553436279296875, -0.2511520087718963623046875, 0.864188015460968017578125)), Vertex<T>(math::Vec<3, T>(-0.956625998020172119140625, -0.251148998737335205078125, -0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(-0.1552149951457977294921875, -0.2511520087718963623046875, -0.955421984195709228515625)), Vertex<T>(math::Vec<3, T>(0.8606979846954345703125, -0.2511509954929351806640625, -0.4428580105304718017578125)), Vertex<T>(math::Vec<3, T>(0.276387989521026611328125, 0.4472199976444244384765625, 0.85064899921417236328125)), Vertex<T>(math::Vec<3, T>(0.48397099971771240234375, 0.502301990985870361328125, 0.71656501293182373046875)), Vertex<T>(math::Vec<3, T>(0.232822000980377197265625, 0.65751898288726806640625, 0.7165629863739013671875)), Vertex<T>(math::Vec<3, T>(-0.723607003688812255859375, 0.4472199976444244384765625, 0.52572500705718994140625)), Vertex<T>(math::Vec<3, T>(-0.531940996646881103515625, 0.502301990985870361328125, 0.681711971759796142578125)), Vertex<T>(math::Vec<3, T>(-0.60954701900482177734375, 0.65751898288726806640625, 0.442856013774871826171875)), Vertex<T>(math::Vec<3, T>(-0.723607003688812255859375, 0.4472199976444244384765625, -0.52572500705718994140625)), Vertex<T>(math::Vec<3, T>(-0.81272900104522705078125, 0.5023009777069091796875, -0.2952379882335662841796875)), Vertex<T>(math::Vec<3, T>(-0.60954701900482177734375, 0.65751898288726806640625, -0.442856013774871826171875)), Vertex<T>(math::Vec<3, T>(0.276387989521026611328125, 0.4472199976444244384765625, -0.85064899921417236328125)), Vertex<T>(math::Vec<3, T>(0.02963900007307529449462890625, 0.502301990985870361328125, -0.86418402194976806640625)), Vertex<T>(math::Vec<3, T>(0.232822000980377197265625, 0.65751898288726806640625, -0.7165629863739013671875)), Vertex<T>(math::Vec<3, T>(0.89442598819732666015625, 0.4472160041332244873046875, 0)), Vertex<T>(math::Vec<3, T>(0.83105099201202392578125, 0.502299010753631591796875, -0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(0.753441989421844482421875, 0.657514989376068115234375, 0)), Vertex<T>(math::Vec<3, T>(-0.232822000980377197265625, -0.65751898288726806640625, 0.7165629863739013671875)), Vertex<T>(math::Vec<3, T>(-0.1624560058116912841796875, -0.85065400600433349609375, 0.4999949932098388671875)), Vertex<T>(math::Vec<3, T>(0.05279000103473663330078125, -0.723612010478973388671875, 0.6881849765777587890625)), Vertex<T>(math::Vec<3, T>(0.1381990015506744384765625, -0.894429028034210205078125, 0.4253210127353668212890625)), Vertex<T>(math::Vec<3, T>(0.26286900043487548828125, -0.5257380008697509765625, 0.809011995792388916015625)), Vertex<T>(math::Vec<3, T>(0.3618049919605255126953125, -0.72361099720001220703125, 0.587778985500335693359375)), Vertex<T>(math::Vec<3, T>(0.531940996646881103515625, -0.502301990985870361328125, 0.681711971759796142578125)), Vertex<T>(math::Vec<3, T>(0.425323009490966796875, -0.85065400600433349609375, 0.3090110123157501220703125)), Vertex<T>(math::Vec<3, T>(0.81272900104522705078125, -0.5023009777069091796875, -0.2952379882335662841796875)), Vertex<T>(math::Vec<3, T>(0.60954701900482177734375, -0.65751898288726806640625, -0.442856013774871826171875)), Vertex<T>(math::Vec<3, T>(0.850647985935211181640625, -0.52573597431182861328125, 0)), Vertex<T>(math::Vec<3, T>(0.67081701755523681640625, -0.72361099720001220703125, -0.16245700418949127197265625)), Vertex<T>(math::Vec<3, T>(0.67081701755523681640625, -0.723609983921051025390625, 0.162458002567291259765625)), Vertex<T>(math::Vec<3, T>(0.425323009490966796875, -0.85065400600433349609375, -0.3090110123157501220703125)), Vertex<T>(math::Vec<3, T>(0.4472109973430633544921875, -0.8944280147552490234375, 9.999999974752427078783512115478515625e-007)), Vertex<T>(math::Vec<3, T>(-0.753441989421844482421875, -0.657514989376068115234375, 0)), Vertex<T>(math::Vec<3, T>(-0.52573001384735107421875, -0.8506519794464111328125, 0)), Vertex<T>(math::Vec<3, T>(-0.638194978237152099609375, -0.72360897064208984375, 0.26286399364471435546875)), Vertex<T>(math::Vec<3, T>(-0.3618009984493255615234375, -0.8944280147552490234375, 0.26286399364471435546875)), Vertex<T>(math::Vec<3, T>(-0.688189029693603515625, -0.52573597431182861328125, 0.4999969899654388427734375)), Vertex<T>(math::Vec<3, T>(-0.4472109973430633544921875, -0.723609983921051025390625, 0.525729000568389892578125)), Vertex<T>(math::Vec<3, T>(-0.48397099971771240234375, -0.502301990985870361328125, 0.71656501293182373046875)), Vertex<T>(math::Vec<3, T>(-0.232822000980377197265625, -0.65751898288726806640625, -0.7165629863739013671875)), Vertex<T>(math::Vec<3, T>(-0.1624560058116912841796875, -0.85065400600433349609375, -0.4999949932098388671875)), Vertex<T>(math::Vec<3, T>(-0.4472109973430633544921875, -0.72361099720001220703125, -0.525726974010467529296875)), Vertex<T>(math::Vec<3, T>(-0.3618009984493255615234375, -0.894429028034210205078125, -0.2628630101680755615234375)), Vertex<T>(math::Vec<3, T>(-0.688189029693603515625, -0.52573597431182861328125, -0.4999969899654388427734375)), Vertex<T>(math::Vec<3, T>(-0.638194978237152099609375, -0.72360897064208984375, -0.2628630101680755615234375)), Vertex<T>(math::Vec<3, T>(-0.83105099201202392578125, -0.502299010753631591796875, -0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(0.36180400848388671875, -0.723612010478973388671875, -0.587778985500335693359375)), Vertex<T>(math::Vec<3, T>(0.138197004795074462890625, -0.894429028034210205078125, -0.4253210127353668212890625)), Vertex<T>(math::Vec<3, T>(0.26286900043487548828125, -0.5257380008697509765625, -0.809011995792388916015625)), Vertex<T>(math::Vec<3, T>(0.0527889989316463470458984375, -0.72361099720001220703125, -0.688185989856719970703125)), Vertex<T>(math::Vec<3, T>(-0.02963900007307529449462890625, -0.502301990985870361328125, -0.86418402194976806640625)), Vertex<T>(math::Vec<3, T>(0.956625998020172119140625, 0.251148998737335205078125, 0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(0.956625998020172119140625, 0.251148998737335205078125, -0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(0.951057970523834228515625, -0, 0.30901300907135009765625)), Vertex<T>(math::Vec<3, T>(1, 0, 0)), Vertex<T>(math::Vec<3, T>(0.947212994098663330078125, -0.2763960063457489013671875, 0.162458002567291259765625)), Vertex<T>(math::Vec<3, T>(0.951057970523834228515625, 0, -0.30901300907135009765625)), Vertex<T>(math::Vec<3, T>(0.947212994098663330078125, -0.2763960063457489013671875, -0.162458002567291259765625)), Vertex<T>(math::Vec<3, T>(0.1552149951457977294921875, 0.2511520087718963623046875, 0.955421984195709228515625)), Vertex<T>(math::Vec<3, T>(0.4360069930553436279296875, 0.2511520087718963623046875, 0.864188015460968017578125)), Vertex<T>(math::Vec<3, T>(-0, -0, 1)), Vertex<T>(math::Vec<3, T>(0.309017002582550048828125, 0, 0.951056003570556640625)), Vertex<T>(math::Vec<3, T>(0.1381990015506744384765625, -0.276398003101348876953125, 0.951054990291595458984375)), Vertex<T>(math::Vec<3, T>(0.587786018848419189453125, 0, 0.809017002582550048828125)), Vertex<T>(math::Vec<3, T>(0.4472160041332244873046875, -0.276398003101348876953125, 0.850647985935211181640625)), Vertex<T>(math::Vec<3, T>(-0.8606979846954345703125, 0.2511509954929351806640625, 0.4428580105304718017578125)), Vertex<T>(math::Vec<3, T>(-0.687159001827239990234375, 0.2511520087718963623046875, 0.6817150115966796875)), Vertex<T>(math::Vec<3, T>(-0.951057970523834228515625, -0, 0.30901300907135009765625)), Vertex<T>(math::Vec<3, T>(-0.80901801586151123046875, 0, 0.58778297901153564453125)), Vertex<T>(math::Vec<3, T>(-0.861802995204925537109375, -0.2763960063457489013671875, 0.4253239929676055908203125)), Vertex<T>(math::Vec<3, T>(-0.587786018848419189453125, 0, 0.809017002582550048828125)), Vertex<T>(math::Vec<3, T>(-0.670818984508514404296875, -0.2763969898223876953125, 0.688190996646881103515625)), Vertex<T>(math::Vec<3, T>(-0.687159001827239990234375, 0.2511520087718963623046875, -0.6817150115966796875)), Vertex<T>(math::Vec<3, T>(-0.8606979846954345703125, 0.2511509954929351806640625, -0.4428580105304718017578125)), Vertex<T>(math::Vec<3, T>(-0.587786018848419189453125, -0, -0.809017002582550048828125)), Vertex<T>(math::Vec<3, T>(-0.80901801586151123046875, -0, -0.58778297901153564453125)), Vertex<T>(math::Vec<3, T>(-0.670818984508514404296875, -0.2763969898223876953125, -0.688190996646881103515625)), Vertex<T>(math::Vec<3, T>(-0.951057970523834228515625, 0, -0.30901300907135009765625)), Vertex<T>(math::Vec<3, T>(-0.861802995204925537109375, -0.2763960063457489013671875, -0.4253239929676055908203125)), Vertex<T>(math::Vec<3, T>(0.4360069930553436279296875, 0.2511520087718963623046875, -0.864188015460968017578125)), Vertex<T>(math::Vec<3, T>(0.1552149951457977294921875, 0.2511520087718963623046875, -0.955421984195709228515625)), Vertex<T>(math::Vec<3, T>(0.587786018848419189453125, -0, -0.809017002582550048828125)), Vertex<T>(math::Vec<3, T>(0.309017002582550048828125, -0, -0.951056003570556640625)), Vertex<T>(math::Vec<3, T>(0.4472160041332244873046875, -0.276398003101348876953125, -0.850647985935211181640625)), Vertex<T>(math::Vec<3, T>(0, 0, -1)), Vertex<T>(math::Vec<3, T>(0.1381990015506744384765625, -0.276398003101348876953125, -0.951054990291595458984375)), Vertex<T>(math::Vec<3, T>(0.6708199977874755859375, 0.2763960063457489013671875, 0.688189983367919921875)), Vertex<T>(math::Vec<3, T>(0.809019029140472412109375, -1.999999994950485415756702423095703125e-006, 0.58778297901153564453125)), Vertex<T>(math::Vec<3, T>(0.688189029693603515625, 0.52573597431182861328125, 0.4999969899654388427734375)), Vertex<T>(math::Vec<3, T>(0.86180400848388671875, 0.27639400959014892578125, 0.425323009490966796875)), Vertex<T>(math::Vec<3, T>(0.83105099201202392578125, 0.502299010753631591796875, 0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(-0.4472160041332244873046875, 0.2763969898223876953125, 0.85064899921417236328125)), Vertex<T>(math::Vec<3, T>(-0.309017002582550048828125, -9.999999974752427078783512115478515625e-007, 0.951056003570556640625)), Vertex<T>(math::Vec<3, T>(-0.26286900043487548828125, 0.5257380008697509765625, 0.809011995792388916015625)), Vertex<T>(math::Vec<3, T>(-0.1381990015506744384765625, 0.2763969898223876953125, 0.951054990291595458984375)), Vertex<T>(math::Vec<3, T>(0.02963900007307529449462890625, 0.502301990985870361328125, 0.86418402194976806640625)), Vertex<T>(math::Vec<3, T>(-0.947212994098663330078125, 0.2763960063457489013671875, -0.162458002567291259765625)), Vertex<T>(math::Vec<3, T>(-1, 9.999999974752427078783512115478515625e-007, 0)), Vertex<T>(math::Vec<3, T>(-0.850647985935211181640625, 0.52573597431182861328125, -0)), Vertex<T>(math::Vec<3, T>(-0.947212994098663330078125, 0.2763969898223876953125, 0.162458002567291259765625)), Vertex<T>(math::Vec<3, T>(-0.81272900104522705078125, 0.5023009777069091796875, 0.2952379882335662841796875)), Vertex<T>(math::Vec<3, T>(-0.1381990015506744384765625, 0.2763969898223876953125, -0.951054990291595458984375)), Vertex<T>(math::Vec<3, T>(-0.3090159893035888671875, -0, -0.951057016849517822265625)), Vertex<T>(math::Vec<3, T>(-0.26286900043487548828125, 0.5257380008697509765625, -0.809011995792388916015625)), Vertex<T>(math::Vec<3, T>(-0.4472149908542633056640625, 0.2763969898223876953125, -0.85064899921417236328125)), Vertex<T>(math::Vec<3, T>(-0.531940996646881103515625, 0.502301990985870361328125, -0.681711971759796142578125)), Vertex<T>(math::Vec<3, T>(0.86180400848388671875, 0.2763960063457489013671875, -0.425321996212005615234375)), Vertex<T>(math::Vec<3, T>(0.809019029140472412109375, 0, -0.58778202533721923828125)), Vertex<T>(math::Vec<3, T>(0.688189029693603515625, 0.52573597431182861328125, -0.4999969899654388427734375)), Vertex<T>(math::Vec<3, T>(0.670821011066436767578125, 0.2763969898223876953125, -0.688189029693603515625)), Vertex<T>(math::Vec<3, T>(0.48397099971771240234375, 0.502301990985870361328125, -0.71656501293182373046875)), Vertex<T>(math::Vec<3, T>(0.077606998383998870849609375, 0.96794998645782470703125, 0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(0.2511470019817352294921875, 0.967948973178863525390625, 0)), Vertex<T>(math::Vec<3, T>(0, 1, 0)), Vertex<T>(math::Vec<3, T>(0.1624560058116912841796875, 0.85065400600433349609375, 0.4999949932098388671875)), Vertex<T>(math::Vec<3, T>(0.3617999851703643798828125, 0.894429028034210205078125, 0.2628630101680755615234375)), Vertex<T>(math::Vec<3, T>(0.44720900058746337890625, 0.723612010478973388671875, 0.5257279872894287109375)), Vertex<T>(math::Vec<3, T>(0.52573001384735107421875, 0.8506519794464111328125, 0)), Vertex<T>(math::Vec<3, T>(0.638194024562835693359375, 0.723609983921051025390625, 0.26286399364471435546875)), Vertex<T>(math::Vec<3, T>(-0.2031809985637664794921875, 0.96794998645782470703125, 0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(-0.425323009490966796875, 0.85065400600433349609375, 0.3090110123157501220703125)), Vertex<T>(math::Vec<3, T>(-0.138197004795074462890625, 0.894429981708526611328125, 0.4253199994564056396484375)), Vertex<T>(math::Vec<3, T>(-0.36180400848388671875, 0.723612010478973388671875, 0.58777797222137451171875)), Vertex<T>(math::Vec<3, T>(-0.05279000103473663330078125, 0.723612010478973388671875, 0.6881849765777587890625)), Vertex<T>(math::Vec<3, T>(-0.2031809985637664794921875, 0.96794998645782470703125, -0.147617995738983154296875)), Vertex<T>(math::Vec<3, T>(-0.425323009490966796875, 0.85065400600433349609375, -0.3090110123157501220703125)), Vertex<T>(math::Vec<3, T>(-0.447210013866424560546875, 0.894429028034210205078125, 0)), Vertex<T>(math::Vec<3, T>(-0.67081701755523681640625, 0.72361099720001220703125, -0.16245700418949127197265625)), Vertex<T>(math::Vec<3, T>(-0.67081701755523681640625, 0.72361099720001220703125, 0.16245700418949127197265625)), Vertex<T>(math::Vec<3, T>(0.077606998383998870849609375, 0.96794998645782470703125, -0.23885299265384674072265625)), Vertex<T>(math::Vec<3, T>(0.1624560058116912841796875, 0.85065400600433349609375, -0.4999949932098388671875)), Vertex<T>(math::Vec<3, T>(-0.138197004795074462890625, 0.894429981708526611328125, -0.4253199994564056396484375)), Vertex<T>(math::Vec<3, T>(-0.05279000103473663330078125, 0.723612010478973388671875, -0.6881849765777587890625)), Vertex<T>(math::Vec<3, T>(-0.36180400848388671875, 0.723612010478973388671875, -0.58777797222137451171875)), Vertex<T>(math::Vec<3, T>(0.3617999851703643798828125, 0.894429028034210205078125, -0.2628630101680755615234375)), Vertex<T>(math::Vec<3, T>(0.638194024562835693359375, 0.723609983921051025390625, -0.26286399364471435546875)), Vertex<T>(math::Vec<3, T>(0.44720900058746337890625, 0.723612010478973388671875, -0.5257279872894287109375)))));
		uint indexes[] = {0, 1, 2, 3, 4, 5, 0, 2, 6, 0, 6, 7, 0, 7, 8, 3, 5, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 3, 9, 22, 10, 12, 23, 13, 15, 24, 16, 18, 25, 19, 21, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 11, 10, 43, 44, 42, 2, 45, 43, 42, 44, 11, 44, 46, 11, 43, 45, 44, 45, 47, 44, 44, 47, 46, 47, 48, 46, 2, 1, 45, 1, 49, 45, 45, 49, 47, 49, 4, 47, 47, 4, 48, 4, 3, 48, 50, 51, 19, 52, 53, 50, 5, 54, 52, 50, 53, 51, 53, 55, 51, 52, 54, 53, 54, 56, 53, 53, 56, 55, 56, 8, 55, 5, 4, 54, 4, 49, 54, 54, 49, 56, 49, 1, 56, 56, 1, 8, 1, 0, 8, 57, 14, 13, 58, 59, 57, 6, 60, 58, 57, 59, 14, 59, 61, 14, 58, 60, 59, 60, 62, 59, 59, 62, 61, 62, 63, 61, 6, 2, 60, 2, 43, 60, 60, 43, 62, 43, 42, 62, 62, 42, 63, 42, 10, 63, 64, 17, 16, 65, 66, 64, 7, 67, 65, 64, 66, 17, 66, 68, 17, 65, 67, 66, 67, 69, 66, 66, 69, 68, 69, 70, 68, 7, 6, 67, 6, 58, 67, 67, 58, 69, 58, 57, 69, 69, 57, 70, 57, 13, 70, 51, 20, 19, 55, 71, 51, 8, 72, 55, 51, 71, 20, 71, 73, 20, 55, 72, 71, 72, 74, 71, 71, 74, 73, 74, 75, 73, 8, 7, 72, 7, 65, 72, 72, 65, 74, 65, 64, 74, 74, 64, 75, 64, 16, 75, 76, 77, 39, 78, 79, 76, 9, 80, 78, 76, 79, 77, 79, 81, 77, 78, 80, 79, 80, 82, 79, 79, 82, 81, 82, 26, 81, 9, 5, 80, 5, 52, 80, 80, 52, 82, 52, 50, 82, 82, 50, 26, 50, 19, 26, 83, 84, 27, 85, 86, 83, 12, 87, 85, 83, 86, 84, 86, 88, 84, 85, 87, 86, 87, 89, 86, 86, 89, 88, 89, 22, 88, 12, 11, 87, 11, 46, 87, 87, 46, 89, 46, 48, 89, 89, 48, 22, 48, 3, 22, 90, 91, 30, 92, 93, 90, 15, 94, 92, 90, 93, 91, 93, 95, 91, 92, 94, 93, 94, 96, 93, 93, 96, 95, 96, 23, 95, 15, 14, 94, 14, 61, 94, 94, 61, 96, 61, 63, 96, 96, 63, 23, 63, 10, 23, 97, 98, 33, 99, 100, 97, 18, 101, 99, 97, 100, 98, 100, 102, 98, 99, 101, 100, 101, 103, 100, 100, 103, 102, 103, 24, 102, 18, 17, 101, 17, 68, 101, 101, 68, 103, 68, 70, 103, 103, 70, 24, 70, 13, 24, 104, 105, 36, 106, 107, 104, 21, 108, 106, 104, 107, 105, 107, 109, 105, 106, 108, 107, 108, 110, 107, 107, 110, 109, 110, 25, 109, 21, 20, 108, 20, 73, 108, 108, 73, 110, 73, 75, 110, 110, 75, 25, 75, 16, 25, 84, 28, 27, 88, 111, 84, 22, 112, 88, 84, 111, 28, 111, 113, 28, 88, 112, 111, 112, 114, 111, 111, 114, 113, 114, 115, 113, 22, 9, 112, 9, 78, 112, 112, 78, 114, 78, 76, 114, 114, 76, 115, 76, 39, 115, 91, 31, 30, 95, 116, 91, 23, 117, 95, 91, 116, 31, 116, 118, 31, 95, 117, 116, 117, 119, 116, 116, 119, 118, 119, 120, 118, 23, 12, 117, 12, 85, 117, 117, 85, 119, 85, 83, 119, 119, 83, 120, 83, 27, 120, 98, 34, 33, 102, 121, 98, 24, 122, 102, 98, 121, 34, 121, 123, 34, 102, 122, 121, 122, 124, 121, 121, 124, 123, 124, 125, 123, 24, 15, 122, 15, 92, 122, 122, 92, 124, 92, 90, 124, 124, 90, 125, 90, 30, 125, 105, 37, 36, 109, 126, 105, 25, 127, 109, 105, 126, 37, 126, 128, 37, 109, 127, 126, 127, 129, 126, 126, 129, 128, 129, 130, 128, 25, 18, 127, 18, 99, 127, 127, 99, 129, 99, 97, 129, 129, 97, 130, 97, 33, 130, 77, 40, 39, 81, 131, 77, 26, 132, 81, 77, 131, 40, 131, 133, 40, 81, 132, 131, 132, 134, 131, 131, 134, 133, 134, 135, 133, 26, 21, 132, 21, 106, 132, 132, 106, 134, 106, 104, 134, 134, 104, 135, 104, 36, 135, 136, 137, 138, 139, 140, 136, 29, 141, 139, 136, 140, 137, 140, 142, 137, 139, 141, 140, 141, 143, 140, 140, 143, 142, 143, 41, 142, 29, 28, 141, 28, 113, 141, 141, 113, 143, 113, 115, 143, 143, 115, 41, 115, 39, 41, 144, 136, 138, 145, 146, 144, 32, 147, 145, 144, 146, 136, 146, 139, 136, 145, 147, 146, 147, 148, 146, 146, 148, 139, 148, 29, 139, 32, 31, 147, 31, 118, 147, 147, 118, 148, 118, 120, 148, 148, 120, 29, 120, 27, 29, 149, 144, 138, 150, 151, 149, 35, 152, 150, 149, 151, 144, 151, 145, 144, 150, 152, 151, 152, 153, 151, 151, 153, 145, 153, 32, 145, 35, 34, 152, 34, 123, 152, 152, 123, 153, 123, 125, 153, 153, 125, 32, 125, 30, 32, 154, 149, 138, 155, 156, 154, 38, 157, 155, 154, 156, 149, 156, 150, 149, 155, 157, 156, 157, 158, 156, 156, 158, 150, 158, 35, 150, 38, 37, 157, 37, 128, 157, 157, 128, 158, 128, 130, 158, 158, 130, 35, 130, 33, 35, 137, 154, 138, 142, 159, 137, 41, 160, 142, 137, 159, 154, 159, 155, 154, 142, 160, 159, 160, 161, 159, 159, 161, 155, 161, 38, 155, 41, 40, 160, 40, 133, 160, 160, 133, 161, 133, 135, 161, 161, 135, 38, 135, 36, 38};
		for(uint i = 0; i < sizeof(indexes) / sizeof(uint); i += 3) {
			tris.append(indexes[i], indexes[i + 1], indexes[i + 2]);
		}
		fr = new FreezedTriangleBuffer(tris.freezed());
	}
	return *fr;
}

template<typename T>
const typename TriangleBuffer<T>::FreezedTriangleBuffer &TriangleBuffer<T>::getCube() {
	static FreezedTriangleBuffer *fr = 0;
	if(!fr) {
		TriangleBuffer<T> tris(VertexBuffer<T>(core::Array<Vertex<T>>(
			Vertex<T>(math::Vec<3, T>(0.5, -0.5, -0.5)), Vertex<T>(math::Vec<3, T>(0.5, -0.5, 0.5)),
			Vertex<T>(math::Vec<3, T>(-0.5, -0.5, 0.5)), Vertex<T>(math::Vec<3, T>(-0.5, -0.5, -0.5)),
			Vertex<T>(math::Vec<3, T>(0.5, 0.5, -0.5)), Vertex<T>(math::Vec<3, T>(0.5, 0.5, 0.5)),
			Vertex<T>(math::Vec<3, T>(-0.5, 0.5, 0.5)), Vertex<T>(math::Vec<3, T>(-0.5, 0.5, -0.5)))));

		uint indexes[] = {0, 1, 2, 0, 2, 3, 4, 7, 6, 4, 6, 5, 0, 4, 5, 0, 5, 1, 1, 5, 6, 1, 6, 2, 2, 6, 7, 2, 7, 3, 4, 0, 3, 4, 3, 7};
		for(uint i = 0; i < sizeof(indexes) / sizeof(uint); i += 3) {
			tris.append(indexes[i], indexes[i + 1], indexes[i + 2]);
		}
		fr = new FreezedTriangleBuffer(tris.freezed());
	}
	return *fr;
}




}
}
}

#endif

#endif // N_GRAPHICS_GL_TRIANGLEBUFFER

