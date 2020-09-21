#include "primitive.h"
#include "screen.h"
#include "AABB.h"
#include "auxiliary.h"
#include "rasterizer.h"

rasterizer::rasterizer(camera *c, screen *s):cam(c), scn(s)
{
}

void
rasterizer::draw_line(const vector4f &begin, const vector4f &end)
{
    auto x1 = begin.x();
    auto y1 = begin.y();
    auto x2 = end.x();
    auto y2 = end.y();

    vector3f line_color = {255, 255, 255};

    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

    dx=x2-x1;
    dy=y2-y1;
    dx1=fabs(dx);
    dy1=fabs(dy);
    px=2*dy1-dx1;
    py=2*dx1-dy1;

    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        scn->set(x, y, line_color);
        for(i=0;x<xe;i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
            scn->set(x, y,line_color);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        scn->set(x, y,line_color);
        for(i=0;y<ye;i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
	    scn->set(x, y, line_color);
        }
    }
}

float f1 = (50 - 0.1) / 2.0;
float f2 = (50 + 0.1) / 2.0;


void
rasterizer::render_triangle(const triangle &tri, const material *m, vector3f viewpos[3])
{
	AABB2f b;
	for (int i = 0; i < 3; i++)
		b.extend(tri.ver[i]);
	auto &v = tri.ver;
	for (int x = b.min.x(); x <= std::ceil(b.max.x()); x++) {
		for (int y = b.min.y(); y <= std::ceil(b.max.y()); y++) {
			vector3f bc, bcw;
			float one_div_z, Z, zp;
			vector2f texcoord(0, 0);
			vector3f normal(0, 0, 0), color(0, 0, 0), view(0, 0, 0);
			bc = barycentric2d(x, y, v);
			if (bc[0] < 0.f || bc[1] < 0.f || bc[2] < 0.f)
				continue;
			one_div_z = 0.f;
			for (int i = 0; i < 3; i++) {
				bcw[i] = bc[i] / v[i].w();
				one_div_z += bcw[i];
			}
			Z = 1 / one_div_z;
			zp = 0.f;
			for (int i = 0; i < 3; i++)
				zp += v[i].z() * bc[i];
			//transform zp to view space
			zp *= Z;
			if (!scn->ztest(x, y, zp))
				continue;
			for (int i = 0; i < 3; i++) {
				view = view + viewpos[i] * bcw[i];
				normal = normal + tri.n[i] * bcw[i];
				texcoord = texcoord + tri.uv[i] * bcw[i];
			}
			auto *sd = m->shader;
			//sd->color = color * Z;
			sd->tex = m->tex;
			sd->view_pos = view * Z;
			sd->normal = normal * Z;
			sd->texcoord = texcoord * Z;
			color = sd->frag();
			scn->set(x, y, color, zp);
		}
	}
}

/*
	p1(x1, y1)	p2(x2, y2)
	N(x3, y3)	T(x2-x1, y2-y1)

	transform with (matrix) transpose([2, 1/2])

	p1'(x1*2, y1/2)	p2'(x2*2, y2/2)
	N'(x3*2,y3/2)	T'((x2-x1)/2, (y2-y1)/2)

	dot((p2'-p1'), N') != 0
	p2'-p1' = T'

	so the T' is always right, but the N' is not

	assume we have another matrix G,
	it can transform the N to correct N',
	let's solve the G

	T * N = 0;
	T'* N' = 0;
	T' = T * M;
	N' = N * G;
	T' * N' = 0;
	(vector)(M*T) * (vector)(G*N) = 0
	=>transpose(M*T) * (G*N) = 0
	=>transpose(T) * transpose(M) * G * N = 0;
	because transpose(T) * N = 0
	so transpose(M) * G = Identity
	G = transpose(M)-1
	G = transpose(M-1)
*/

void
rasterizer::render_mesh(const mesh &m)
{
	std::vector<triangle> tris;
	m.fetch(tris);
	matrix4f mv = cam->view() * m.model();
	matrix4f mvp = cam->projection() * mv;
	matrix4f normal_trans = mv.inverse().transpose();
	float width = scn->getsize().x();
	float height = scn->getsize().y();
	for (auto &t:tris) {
		vector3f view_pos[3];
		for (int i = 0; i < 3; i++) {
			view_pos[i] = (mv * t.ver[i]).head<3>();
			t.ver[i] = mvp * t.ver[i];
		}
		//Homogeneous division
		for (int i = 0; i < 3; i++) {
			float w = t.ver[i].w();
			t.ver[i].x() /= w;
			t.ver[i].y() /= w;
			t.ver[i].z() /= w;
		}
		for (int i = 0; i < 3; i++) {
			t.n[i] = (normal_trans * tovector4f(t.n[i], 0.f)).head<3>();
		}
		for (int i = 0; i < 3; i++) {
			auto &v = t.ver[i];
			v.x() = 0.5*width*(v.x()+1.0);
			v.y() = 0.5*height*(v.y()+1.0);
		}

		render_triangle(t, m.getmaterial(), view_pos);
	}
}

void
rasterizer::render(const scene &s)
{
	auto &objs = s.getobjs();
	for (auto &m:objs)
		render_mesh(*m);
	return ;
}


