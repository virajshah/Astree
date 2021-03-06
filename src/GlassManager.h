// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef GlassManager_
#define GlassManager_

#include <string>
#include <vector>
using namespace std;

class Glass;

class GlassManager
{
public:
    static GlassManager& singleton();
    Glass* create(string sMaterial) const;
    static void destroy(Glass* pMaterial);

    void list_available(vector<string>& vsAvailable);

    bool exist(const string& sGlass) const;

    unsigned int solid_color(const string& sMaterial);

    void inject(Glass* pGlass); //take ownership of pGlass

private:
    GlassManager();
    ~GlassManager();

    static GlassManager* _pGlassManager;
    vector<Glass*> _vGlass; // TODO use map
};

#endif
