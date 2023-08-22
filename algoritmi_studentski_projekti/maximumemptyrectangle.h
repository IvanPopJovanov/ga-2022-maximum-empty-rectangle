#ifndef MAXIMUMEMPTYRECTANGLE_H
#define MAXIMUMEMPTYRECTANGLE_H

#include <algoritambaza.h>
#include <QObject>

class MaximumEmptyRectangle : public AlgoritamBaza
{
public:
    MaximumEmptyRectangle(QWidget *pCrtanje,
                    int pauzaKoraka,
                    const bool &naivni = false,
                    std::string imeDatoteke = "",
                    int brojTacaka = BROJ_SLUCAJNIH_OBJEKATA);

    void pokreniAlgoritam() final;
    void crtajAlgoritam(QPainter *painter) const final;
    void pokreniNaivniAlgoritam() final;
    void crtajNaivniAlgoritam(QPainter *painter) const final;

private:
    std::vector<QPoint> _tacke;
    unsigned int _n;

    int Al, Ar, Ab, At;
    int Ml = 0, Mr = 0, Mb = 0, Mt = 0;
    int Cl = 0, Cr = 0, Cb = 0, Ct = 0;

    QPoint checking_point;
    bool currently_checking = false;

    struct sdh;
    QPoint findSDH(int Tl, int Tr);
    void deactivateSDH(sdh *v, QPoint P);
    QPoint deleteSDH();
    sdh* constructSDH(std::vector<QPoint>& points, int left, int right);
};

#endif // MAXIMUMEMPTYRECTANGLE_H
