#include "../MyMath/MyMath.h"
#include "Collision.h"


bool Collision::CircleAndBox(const Circle& c, const Box& b)
{
	if (c.pos.x + c.r <= b.x ||
		c.pos.y + c.r <= b.y ||
		b.x + b.w <= c.pos.x - c.r ||
		b.y + b.h <= c.pos.y - c.r)
		return false;
	else
		return true;

}

bool Collision::BoxAndBox(const Box& b1, const Box& b2)
{
	if (b1.x < b2.x + b2.w &&
		b2.x < b1.x + b1.w &&
		b1.y < b2.y + b2.h &&
		b2.y < b1.y + b1.h)
	{
		return true;
	}
	return false;
}

bool Collision::CircleAndCircle(const Circle& c1, const Circle& c2)
{
	if (((c1.pos.x - c2.pos.x) *(c1.pos.x - c2.pos.x)) + ((c1.pos.y - c2.pos.y) * (c1.pos.y - c2.pos.y)) <= (c1.r + c2.r) * (c1.r + c2.r))
	{
		return true;
	}
	return false;
}

bool Collision::CircleAndSlope(const Circle& c, const Line& l)
{
	// 線分ベクトルと円中心へのベクトルの外積による判定
	if ((c.pos.x + c.r - l.p1.x) * (l.p2.y - l.p1.y) - (l.p2.x - l.p1.x) * (c.pos.y + c.r - l.p1.y) <= 0 &&
		!(c.pos.x <= l.p1.x) && !(c.pos.x >= l.p2.x))
	{
		return true;
	}
	else
		return false;
}

bool Collision::BoxAndSlope(const Box& b, const Line& l)
{
	if ((b.x + b.w - l.p1.x) * (l.p2.y - l.p1.y) - (l.p2.x - l.p1.x) * (b.y + b.h - l.p1.y) <= 0 &&
		!(b.x + b.w <= l.p1.x) && !(b.x >= l.p2.x))
	{
		return true;
	}
	else
		return false;
}

bool Collision::LineAndLine(const Line& l1, const Line& l2)
{
	{
		const float baseX = l2.p2.x - l2.p1.x;
		const float baseY = l2.p2.y - l2.p1.y;
		const float sub1X = l1.p1.x - l2.p1.x;
		const float sub1Y = l1.p1.y - l2.p1.y;
		const float sub2X = l1.p2.x - l2.p1.x;
		const float sub2Y = l1.p2.y - l2.p1.y;

		const float bs1 = baseX * sub1Y - baseY * sub1X;
		const float bs2 = baseX * sub2Y - baseY * sub2X;
		const float re = bs1 * bs2;
		if (re > 0)
		{
			return false;
		}
	}
	{
		const float baseX = l1.p2.x - l1.p1.x;
		const float baseY = l1.p2.y - l1.p1.y;
		const float sub1X = l2.p1.x - l1.p1.x;
		const float sub1Y = l2.p1.y - l1.p1.y;
		const float sub2X = l2.p2.x - l1.p1.x;
		const float sub2Y = l2.p2.y - l1.p1.y;

		const float bs1 = baseX * sub1Y - baseY * sub1X;
		const float bs2 = baseX * sub2Y - baseY * sub2X;
		const float re = bs1 * bs2;
		if (re > 0)
		{
			return false;
		}
	}
	return true;
}

bool Collision::CircleAndTriangle(const Circle& c, const Triangle& t)
{
	//三角形を線分で判定
	Line T_R(t.p1.x, t.p1.y, t.p2.x, t.p2.y);		//上から右下
	Line T_L(t.p1.x, t.p1.y, t.p3.x, t.p3.y);		//上から左下
	Line L_R(t.p2.x, t.p2.y, t.p3.x, t.p3.y);		//底辺

	if (CirecleAndLine(c, T_R))
	{
		return true;
	}
	if (CirecleAndLine(c, T_L))
	{
		return true;
	}
	if (CirecleAndLine(c, L_R))
	{
		return true;
	}

	//内側にあるか外積で判定
	Vec AB = MATH::Sub_Vector(t.p3,t.p2);
	Vec BP = MATH::Sub_Vector(c.pos,t.p3);
	Vec BC = MATH::Sub_Vector(t.p1,t.p3);
	Vec CP = MATH::Sub_Vector(c.pos,t.p1);
	Vec CA = MATH::Sub_Vector(t.p2,t.p1);
	Vec AP = MATH::Sub_Vector(c.pos,t.p2);

	float cross1 = MATH::Cross2D(AB, BP);
	float cross2 = MATH::Cross2D(BC,CP);
	float cross3 = MATH::Cross2D(CA, AP);
	//外積ベクトルの向きがそろっていない場合、点は外にある
	const bool crossDirCheck = (cross1 >= 0 && cross2 > 0 && cross3 > 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0);

	if (crossDirCheck)
	{
		return true;
	}

	return false;
}

bool Collision::CirecleAndLine(const Circle& c, const Line& l)
{
	const Vec A = { c.pos.x - l.p1.x,c.pos.y - l.p1.y };	//線分の始点から円の中心点までのベクトルA
	const Vec B = { l.p2.x - l.p1.x,l.p2.y - l.p1.y };		//線分の始点から線分の終点までのベクトルB
	const Vec C = { c.pos.x - l.p2.x,c.pos.y - l.p2.y };	//線分の終点から円の中心点までのベクトルC

	//円の中心が線分の中（端点の間）に入っている
	if (MATH::Dot2D(A, B)*MATH::Dot2D(B, C) <= 0)
	{
		//線分のベクトル（ベクトルB）を単位ベクトルに変換
		const Vec temp = MATH::Normalize(B);
		//円の中心と線分の距離を外積を使って計算
		float dist = MATH::Cross2D(A, temp);
		//外積がマイナス値だったら符号を変える
		if (dist < 0) { dist *= -1; }
		//半径との比較
		if (dist <= c.r)
		{
			return true;
		}
	}
	else 
	{
		//端点との距離をそれぞれ計算
		if (MATH::Length(A) <= c.r || MATH::Length(C) <= c.r)
		{
			return true;
		}	
	}
	return false;
}

bool  Collision::CircleAndRect(const Circle& c, const Rect& r)
{
	//四角形を線分で判定
	const Line A(r.p1, r.p2);
	const Line B(r.p2, r.p3);
	const Line C(r.p3, r.p4);
	const Line D(r.p4, r.p1);

	if (CirecleAndLine(c, A))
	{
		return true;
	}
	if (CirecleAndLine(c, B))
	{
		return true;
	}
	if (CirecleAndLine(c, C))
	{
		return true;
	}

	if (CirecleAndLine(c, D))
	{
		return true;
	}
	//内側にあるか外積で判定
	const Triangle t1(r.p1,r.p2,r.p3);
	const Triangle t2(r.p3,r.p4,r.p1);
	if (CircleAndTriangle(c, t1))
	{
		return true;
	}
	if (CircleAndTriangle(c, t2))
	{
		return true;
	}
	return false;
}
bool Collision::BoxAndLine(const Box& b, const Line& l)
{
	//四角形を線分で判定
	const Line L_U(b.x, b.y, b.x, b.y + b.h);					//左上~左下
	const Line R_U(b.x + b.w, b.y, b.x + b.w, b.y + b.h);		//右上~右下
	const Line L_D(b.x, b.y, b.x + b.w, b.y);					//左上~右上
	const Line R_D(b.x, b.y + b.h, b.x + b.w, b.y + b.h);		//左下~右下
	
	if (LineAndLine(L_U, l))
	{
		return true;
	}
	if (LineAndLine(R_U, l))
	{
		return true;
	}
	if (LineAndLine(L_D, l))
	{
		return true;
	}
	if (LineAndLine(R_D, l))
	{
		return true;
	}

	return false;
}

bool  Collision::TriangleAndLine(const Triangle& t, const Line& l)
{
	//三角形を線分で判定
	Line T_R(t.p1.x, t.p1.y, t.p2.x, t.p2.y);		//上から右下
	Line T_L(t.p1.x, t.p1.y, t.p3.x, t.p3.y);		//上から左下
	Line L_R(t.p2.x, t.p2.y, t.p3.x, t.p3.y);		//底辺

	if (LineAndLine(T_R, l))
	{
		return true;
	}
	if (LineAndLine(T_L, l))
	{
		return true;
	}
	if (LineAndLine(L_R, l))
	{
		return true;
	}
	return false;
}

