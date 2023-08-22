#include "maximumemptyrectangle.h"
#include <algorithm>

MaximumEmptyRectangle::MaximumEmptyRectangle(QWidget *pCrtanje,
                                                 int pauzaKoraka,
                                                 const bool &naivni,
                                                 std::string imeDatoteke,
                                                 int brojTacaka)
    : AlgoritamBaza(pCrtanje, pauzaKoraka, naivni), _n(0)
{
    if (imeDatoteke == "")
        _tacke = generisiNasumicneTacke(brojTacaka);
    else
        _tacke = ucitajPodatkeIzDatoteke(imeDatoteke);

    if (_pCrtanje)
    {
        Ar = _pCrtanje->width() - 100;
        At = _pCrtanje->height() - 100;
    }
    else
    {
        Ar = CANVAS_WIDTH;
        At = CANVAS_HEIGHT;
    }

    Al = 100;
    Ab = 100;
}

void tryRR(int r, int l, int t, int b) {}

struct MaximumEmptyRectangle::sdh {
    sdh *lchild, *rchild, *parent;
    QPoint sol; // the point (X, Y) whose Y-coordinate is the largest among the points that are currently in the active leaves of the subtree
    int lelem, relem; // the smallest and the largest X-coordinates of the active points stored in the leaves of the subtree
} *T;

QPoint MaximumEmptyRectangle::findSDH(int Tl, int Tr) {
    sdh *v = T; // 1
    QPoint P = QPoint(0, Ab); // 1
    while(true) {
        if(Tl < v->lelem && Tr > v->relem) { // 2
            P = v->sol; // 2.1
            return P; // 2.2
        }
        if(v->lchild == nullptr && v->rchild == nullptr) { // 3
            return P; // 3
        }
        if(Tl >= v->lchild->relem) { // 4
            v = v->rchild; // 4.1
            continue; // 4.2
        }
        if(Tr <= v->rchild->lelem) { // 5
            v = v->lchild; // 5.1
            continue; // 5.2
        }
        sdh *u = v->lchild, *w = v->rchild; // 6
        bool skip = false;
        if(Tl < u->lelem) { // 7
            if(u->sol.y() > P.y()) // 7.1
                P = u->sol;
            skip = true; // 7.2
        }
        while(!skip && u && (u->lchild || u->rchild)) { // 8
            if(u->rchild && Tl < u->rchild->lelem) { // 9
                if(u->rchild->sol.y() > P.y()) // 9.1
                    P = u->rchild->sol;
                u = u->lchild; // 9.2
            }
            else {
                u = u->rchild;
            }
        }
        if(Tr > w->relem) { // 10
            if(w->sol.y() > P.y()) // 10.1
                P = w->sol;
            return P; // 10.2
        }
        while(w && (w->lchild || w->rchild)) { // 11
            if(w->lchild && Tr > w->lchild->relem) { // 12
                if(w->lchild->sol.y() > P.y()) // 12.1
                    P = w->lchild->sol;
                w = w->rchild; // 12.2
            }
            else {
                w = w->lchild;
            }
        }
        return P;
    }
}

void MaximumEmptyRectangle::deactivateSDH(sdh *v, QPoint P) {
//    if(v->lchild && P.x() <= v->lchild->relem) {
//        deactivateSDH(v->lchild, P);
//    }
//    else if(v->rchild && P.x() >= v->rchild->lelem) {
//        deactivateSDH(v->rchild, P);
//    }
//    else {
//        v->sol.setY(Ab);
//        return;
//    }

    if(v->lchild && P == v->lchild->sol) {
        deactivateSDH(v->lchild, P);
    }
    else if(v->rchild && P == v->rchild->sol) {
        deactivateSDH(v->rchild, P);
    }
    else {
        v->sol.setY(Ab);
        return;
    }

    v->sol.setY(-1);

    if (v->lchild) {
        v->lchild->parent = v;
        if(v->lchild->sol.y() > v->sol.y())
            v->sol = v->lchild->sol;
    }

    if (v->rchild) {
        v->rchild->parent = v;
        if(v->rchild->sol.y() > v->sol.y())
            v->sol = v->rchild->sol;
    }
}

QPoint MaximumEmptyRectangle::deleteSDH() {
    sdh *v = T;
    QPoint P = v->sol;
    deactivateSDH(v, P);
    return P;
}

MaximumEmptyRectangle::sdh* MaximumEmptyRectangle::constructSDH(std::vector<QPoint>& points, int left, int right) { // ova funkcija ocekuje da je points sortiran po X koordinati
    if(left > right) {
        return nullptr;
    }
    int mid = (left + right) / 2;
    sdh* node = new sdh();
    node->sol = points[mid];
    node->lelem = points[left].x();
    node->relem = points[right].x();

    if(left == right) {
        node->lchild = nullptr;
        node->rchild = nullptr;
        return node;
    }

    node->lchild = constructSDH(points, left, mid);
    node->rchild = constructSDH(points, mid + 1, right);

    if (node->lchild) {
        node->lchild->parent = node;
        if(node->lchild->sol.y() > node->sol.y())
            node->sol = node->lchild->sol;
    }

    if (node->rchild) {
        node->rchild->parent = node;
        if(node->rchild->sol.y() > node->sol.y())
            node->sol = node->rchild->sol;
    }

    return node;
}

struct LX {
    LX *next, *before;
    int x;
};

void MaximumEmptyRectangle::pokreniAlgoritam()
{
    std::sort(_tacke.begin(), _tacke.end(), [&](const QPoint& lhs, const QPoint& rhs) {
        return lhs.x() < rhs.x();
    });

    std::vector<int> x_sorted = {};
    x_sorted.push_back(Al);
    for(const QPoint& pt : _tacke) {
        x_sorted.push_back(pt.x());
    }
    x_sorted.push_back(Ar);

    Mt = At;
    Mb = Ab;
    Ct = At;
    Cb = Ab;
    for(int i = 1; i < (int)x_sorted.size(); ++i) {
        Cr = x_sorted[i];
        Cl = x_sorted[i-1];
        AlgoritamBaza_updateCanvasAndBlock();
        if(x_sorted[i] - x_sorted[i-1] > Mr - Ml) { // 1
            Mr = x_sorted[i];
            Ml = x_sorted[i-1];
        }
    }

    int maxr = (Mr - Ml)*(Mt - Mb); // 2

    T = constructSDH(_tacke, 0, _tacke.size()-1); // 3

    currently_checking = true;

    for(int i = 0; i < (int)_tacke.size(); ++i) { // 4
        int Tl = Al, Tr = Ar, Y = At; // 5
        QPoint P = deleteSDH(); // 6
        while(Y != Ab) { // 7
            QPoint Pp = findSDH(Tl, Tr); // 7.1
            checking_point = Pp;
            AlgoritamBaza_updateCanvasAndBlock();

            Y = Pp.y();

            Cr = Tr;
            Cl = Tl;
            Ct = P.y();
            Cb = Pp.y();
            AlgoritamBaza_updateCanvasAndBlock();
            if((Tr - Tl)*(P.y()-Pp.y()) > maxr) {
                Mr = Tr;
                Ml = Tl;
                Mt = P.y();
                Mb = Pp.y();
                maxr = (Mr - Ml)*(Mt - Mb); // 7.2
            }

            if(Pp.x() > P.x()) { // 7.3
                Tr = Pp.x();
            } else {
                Tl = Pp.x();
            }
        }
    }

    currently_checking = false;

    // Construct LX, NEXT, BEFOREJ Y and PYX. // 8
    auto _tacke_y_sorted = _tacke;
    std::sort(_tacke_y_sorted.begin(), _tacke_y_sorted.end(), [&](const QPoint& lhs, const QPoint& rhs) {
        return lhs.y() < rhs.y();
    });

    std::vector<int> LY;
    std::vector<LX*> PYX;
    for(const QPoint& pt : _tacke_y_sorted) {
        LY.push_back(pt.y());
        PYX.push_back(nullptr);
    }

    LX *lx = new LX();
    lx->x = Al;
    for(const QPoint& pt : _tacke) {
        LX *lxt = new LX();
        lxt->x = pt.x();
        lxt->before = lx;
        lx->next = lxt;
        lx = lxt;

        int bsl = 0, bsr = _tacke_y_sorted.size();
        int pos = (bsl + bsr) / 2;
        while(bsl<bsr) {
            pos = (bsl + bsr) / 2;
            if(_tacke_y_sorted[pos].y() >= pt.y()) {
                bsr = pos;
            }
            else {
                bsl = pos+1;
            }
        }
        while(_tacke_y_sorted[pos].x() != pt.x() || PYX[pos])
            pos++;
        PYX[pos] = lxt;
    }
    LX *lxt = new LX();
    lxt->x = Ar;
    lxt->before = lx;
    lx->next = lxt;
    lx = lxt;

    // 8 END

    for(int i = 0; i < (int)_tacke.size(); ++i) { // 9
        int Ri = PYX[i]->next->x; // 10
        int Li = PYX[i]->before->x; // 11

        Cr = Ri;
        Cl = Li;
        Ct = At;
        Cb = LY[i];
        AlgoritamBaza_updateCanvasAndBlock();
        if((Ri - Li)*(At-LY[i]) > maxr) {
            Mr = Ri;
            Ml = Li;
            Mt = At;
            Mb = LY[i];
            maxr = (Mr - Ml)*(Mt - Mb); // 12
        }

        PYX[i]->before->next = PYX[i]->next; // 13
        PYX[i]->next->before = PYX[i]->before; // 13
    }

    AlgoritamBaza_updateCanvasAndBlock();
}

void MaximumEmptyRectangle::crtajAlgoritam(QPainter *painter) const
{
    if (!painter) return;

    QPen p = painter->pen();

    p.setWidth(1);
    p.setColor(Qt::gray);
    painter->setPen(p);

    painter->drawLine(Al, Ab, Ar, Ab);
    painter->drawLine(Ar, Ab, Ar, At);
    painter->drawLine(Ar, At, Al, At);
    painter->drawLine(Al, At, Al, Ab);

    p.setWidth(3);
    p.setColor(Qt::red);
    painter->setPen(p);

    painter->drawLine(Ml, Mb, Mr, Mb);
    painter->drawLine(Mr, Mb, Mr, Mt);
    painter->drawLine(Mr, Mt, Ml, Mt);
    painter->drawLine(Ml, Mt, Ml, Mb);

    p.setWidth(3);
    p.setColor(Qt::blue);
    painter->setPen(p);

    painter->drawLine(Cl, Cb, Cr, Cb);
    painter->drawLine(Cr, Cb, Cr, Ct);
    painter->drawLine(Cr, Ct, Cl, Ct);
    painter->drawLine(Cl, Ct, Cl, Cb);

    p.setWidth(10);
    p.setColor(Qt::black);
    painter->setPen(p);

    for(const QPoint &pt : _tacke)
    {
        painter->drawPoint(pt);
    }

    p.setWidth(20);
    p.setColor(Qt::blue);
    painter->setPen(p);

    if(currently_checking) {
        painter->drawPoint(checking_point);
    }
}

void MaximumEmptyRectangle::pokreniNaivniAlgoritam()
{
    std::sort(_tacke.begin(), _tacke.end(), [&](const QPoint& lhs, const QPoint& rhs) {
        return lhs.x() < rhs.x();
    });

    std::vector<int> x_sorted = {};
    x_sorted.push_back(Al);
    for(const QPoint& pt : _tacke) {
        x_sorted.push_back(pt.x());
    }
    x_sorted.push_back(Ar);

    Mt = At;
    Mb = Ab;
    Ct = At;
    Cb = Ab;
    for(int i = 1; i < (int)x_sorted.size(); ++i) {
        Cr = x_sorted[i];
        Cl = x_sorted[i-1];
        AlgoritamBaza_updateCanvasAndBlock();
        if(x_sorted[i] - x_sorted[i-1] > Mr - Ml) { // 1
            Mr = x_sorted[i];
            Ml = x_sorted[i-1];
        }
    }

    int maxr = (Mr - Ml)*(Mt - Mb); // 2

    std::sort(_tacke.begin(), _tacke.end(), [&](const QPoint& lhs, const QPoint& rhs) { // 3
        return lhs.y() > rhs.y();
    });

    for(int i = 0; i < (int)_tacke.size(); ++i) { // 9
        int Ri = Ar;
        int Li = Al;
        for(int j = 0; j < i; ++j) {
            if(_tacke[j].x() > _tacke[i].x() && _tacke[j].x() < Ri) { // 10
                Ri = _tacke[j].x();
            } else if(_tacke[j].x() < _tacke[i].x() && _tacke[j].x() > Li) { // 11
                Li = _tacke[j].x();
            }
        }
        Cr = Ri;
        Cl = Li;
        Ct = At;
        Cb = _tacke[i].y();
        AlgoritamBaza_updateCanvasAndBlock();
        if((Ri - Li)*(At-_tacke[i].y()) > maxr) {
            Mr = Ri;
            Ml = Li;
            Mt = At;
            Mb = _tacke[i].y();
            maxr = (Mr - Ml)*(Mt - Mb); // 12
        }
    }

    currently_checking = true;

    for(int i = 0; i < (int)_tacke.size(); ++i) { // 4
        int Tl = Al, Tr = Ar; // 5
        for(int j = i+1; j < (int)_tacke.size(); ++j) { // 6
            checking_point = _tacke[j];
            AlgoritamBaza_updateCanvasAndBlock()
            if(Tl < _tacke[j].x() && _tacke[j].x() < Tr) { // 7
                Cr = Tr;
                Cl = Tl;
                Ct = _tacke[i].y();
                Cb = _tacke[j].y();
                AlgoritamBaza_updateCanvasAndBlock();
                if((Tr - Tl)*(_tacke[i].y()-_tacke[j].y()) > maxr) {
                    Mr = Tr;
                    Ml = Tl;
                    Mt = _tacke[i].y();
                    Mb = _tacke[j].y();
                    maxr = (Mr - Ml)*(Mt - Mb); // 7.1
                }
                if(_tacke[j].x() > _tacke[i].x()) { // 7.2
                    Tr = _tacke[j].x();
                } else {
                    Tl = _tacke[j].x();
                }
            }
        }
        Cr = Tr;
        Cl = Tl;
        Ct = _tacke[i].y();
        Cb = Ab;
        AlgoritamBaza_updateCanvasAndBlock();
        if((Tr - Tl)*(_tacke[i].y()-Ab) > maxr) {
            Mr = Tr;
            Ml = Tl;
            Mt = _tacke[i].y();
            Mb = Ab;
            maxr = (Mr - Ml)*(Mt - Mb); // 8
        }

    }

    currently_checking = false;

    AlgoritamBaza_updateCanvasAndBlock();
}

void MaximumEmptyRectangle::crtajNaivniAlgoritam(QPainter *painter) const
{
    if (!painter) return;

    QPen p = painter->pen();

    p.setWidth(1);
    p.setColor(Qt::gray);
    painter->setPen(p);

    painter->drawLine(Al, Ab, Ar, Ab);
    painter->drawLine(Ar, Ab, Ar, At);
    painter->drawLine(Ar, At, Al, At);
    painter->drawLine(Al, At, Al, Ab);

    p.setWidth(3);
    p.setColor(Qt::red);
    painter->setPen(p);

    painter->drawLine(Ml, Mb, Mr, Mb);
    painter->drawLine(Mr, Mb, Mr, Mt);
    painter->drawLine(Mr, Mt, Ml, Mt);
    painter->drawLine(Ml, Mt, Ml, Mb);

    p.setWidth(3);
    p.setColor(Qt::blue);
    painter->setPen(p);

    painter->drawLine(Cl, Cb, Cr, Cb);
    painter->drawLine(Cr, Cb, Cr, Ct);
    painter->drawLine(Cr, Ct, Cl, Ct);
    painter->drawLine(Cl, Ct, Cl, Cb);

    p.setWidth(10);
    p.setColor(Qt::black);
    painter->setPen(p);

    for(const QPoint &pt : _tacke)
    {
        painter->drawPoint(pt);
    }

    p.setWidth(20);
    p.setColor(Qt::blue);
    painter->setPen(p);

    if(currently_checking) {
        painter->drawPoint(checking_point);
    }
}
