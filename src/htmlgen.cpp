/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include <stdlib.h>

#include "qtbc.h"
#include <qdir.h>
#include "message.h"
#include "htmlgen.h"
#include "config.h"
#include "util.h"
#include "doxygen.h"
#include "logos.h"
#include "diagram.h"
#include "version.h"
#include "dot.h"

#define GROUP_COLOR "#ff8080"

//#define DBG_HTML(x) x;
#define DBG_HTML(x) 

HtmlHelp *HtmlGenerator::htmlHelp = 0;

HtmlGenerator::HtmlGenerator() : OutputGenerator()
{
  if (!Config::headerFile.isEmpty()) header=fileToString(Config::headerFile);
  if (!Config::footerFile.isEmpty()) footer=fileToString(Config::footerFile);
  dir=Config::htmlOutputDir;
  col=0;  
}

HtmlGenerator::~HtmlGenerator()
{
}

void HtmlGenerator::append(const OutputGenerator *g)
{
  t << g->getContents();
  col+=((HtmlGenerator *)g)->col;
}

void HtmlGenerator::init()
{
  QDir d(Config::htmlOutputDir);
  if (!d.exists() && !d.mkdir(Config::htmlOutputDir))
  {
    err("Could not create output directory %s\n",Config::htmlOutputDir.data());
    exit(1);
  }
  writeLogo(Config::htmlOutputDir);
  writeNullImage(Config::htmlOutputDir);
}

void HtmlGenerator::startFile(const char *name,const char *title,bool external)
{
  QCString fileName=name;
  lastTitle=title;
  if (fileName.right(5)!=".html") fileName+=".html";
  startPlainFile(fileName);
  lastFile = fileName;
  if (header.isEmpty()) 
  {
    t << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
         "<html><head><meta name=\"robots\" content=\"noindex\">\n"
         "<title>" << title << "</title>\n";
    t << "<link ";
    if (external) 
      t << "doxygen=\"_doc:\" href=\"/";
    else
      t << "href=\"";
    if (Config::htmlStyleSheet.isEmpty())
    {
      t << "doxygen.css";
    }
    else
    {
      t << Config::htmlStyleSheet;
    }
    t << "\" rel=\"stylesheet\" type=\"text/css\">\n"
         "</head><body bgcolor=\"#ffffff\">\n";
  }
  else
  {
    t << substituteKeywords(header,lastTitle);
  }
  t << "<!-- Generated by Doxygen " << versionString << " on " 
    << dateToString(TRUE) << " -->" << endl;
}

void HtmlGenerator::startQuickIndexItem(const char *s,const char *l)
{
  QCString *dest;
  if (s) 
  {
    t << "<a class=\"qindexRef\" ";
    t << "doxygen=\"" << s << ":";
    if ((dest=tagDestinationDict[s])) t << *dest << "/";
    t << "\" ";
  }
  else
  {
    t << "<a class=\"qindex\" ";
  }
  t << "href=\""; 
  if (s)
  {
    if ((dest=tagDestinationDict[s])) t << *dest << "/";
  }
  t << l << "\">";
}

void HtmlGenerator::endQuickIndexItem()
{
  t << "</a> &nbsp; ";
}

void HtmlGenerator::writeFooter(int part,bool external)
{
  switch (part)
  {
    case 0:
      if (footer.isEmpty())
        t << "<hr><address><small>";
      else
        t << substituteKeywords(footer,lastTitle);
      break;
    case 1:
      if (footer.isEmpty())
      {
        t << endl << "<a href=\"http://www.stack.nl/~dimitri/doxygen/index.html\">";
        t << endl << "<img ";
        if (external)
        {
          t << "doxygen=\"_doc:\" src=\"/";
        }
        else
        {
          t << "src=\"";
        }
        t << "doxygen.gif\" alt=\"doxygen\" " 
          << "align=center border=0 " << endl << 
             "width=118 height=53></a> " << versionString <<" ";
      }
      break;
    default:
      if (footer.isEmpty())
        t << " <a href=\"mailto:dimitri@stack.nl\">Dimitri van Heesch</a>,\n &copy; "
             "1997-2000</small></address>\n</body>\n</html>\n";
      break;
      
  }
}

void HtmlGenerator::endFile()
{
  endPlainFile();
}

void HtmlGenerator::startProjectNumber()
{
  t << "<h3 align=center>";
}

void HtmlGenerator::endProjectNumber()
{
  t << "</h3>";
}

void HtmlGenerator::writeStyleInfo(int part)
{
  if (part==0)
  {
    if (Config::htmlStyleSheet.isEmpty()) // write default style sheet
    {
      startPlainFile("doxygen.css"); 
      
      // alternative, cooler looking titles
      //t << "H1 { text-align: center; border-width: thin none thin none;" << endl;
      //t << "     border-style : double; border-color : blue; padding-left : 1em; padding-right : 1em }" << endl;

      t << "H1 { text-align: center; }" << endl;
      t << "A.qindex {}" << endl;
      t << "A.qindexRef {}" << endl;
      t << "A.el { text-decoration: none; font-weight: bold }" << endl;
      t << "A.elRef { font-weight: bold }" << endl;
      t << "A.code { text-decoration: none; font-weight: normal; color: #4444ee }" << endl;
      t << "A.codeRef { font-weight: normal; color: #4444ee }" << endl;
      t << "DL.el { margin-left: -1cm }" << endl;
      t << "DIV.fragment { width: 100%; border: none; background-color: #eeeeee }" << endl;
      t << "DIV.in { margin-left: 16 }" << endl;
      t << "DIV.ah { background-color: black; margin-bottom: 3; margin-top: 3 }" << endl;
      t << "TD.md { background-color: #f2f2ff }" << endl;
      t << "DIV.groupHeader { margin-left: 16; margin-top: 12; margin-bottom: 6; font-weight: bold }" << endl;
      t << "DIV.groupText { margin-left: 16; font-style: italic; font-size: smaller }" << endl;
      //t << "TD.groupLine { background-color: #3080ff }" << endl;
      t << endl;
      endPlainFile();
    }
    else // write user defined style sheet
    {
      startPlainFile(Config::htmlStyleSheet);
      t << fileToString(Config::htmlStyleSheet);
      endPlainFile();
    }
  }
}

void HtmlGenerator::startDoxyAnchor(const char *,const char *,
                                    const char *anchor, const char *name)
{
  t << "<a name=\"" << anchor << "\" doxytag=\"" << name << "\">";
}

void HtmlGenerator::endDoxyAnchor()
{
  t << "</a>" << endl;
}

void HtmlGenerator::newParagraph()
{
  t << endl << "<p>" << endl;
}

void HtmlGenerator::writeString(const char *text)
{
  t << text;
}

void HtmlGenerator::writeIndexItem(const char *ref,const char *f,
                                   const char *name)
{
  QCString *dest;
  t << "<li>";
  if (ref || f)
  {
    if (ref) 
    {
      t << "<a class=\"elRef\" ";
      t << "doxygen=\"" << ref << ":";
      if ((dest=tagDestinationDict[ref])) t << *dest << "/";
      t << "\" ";
    }
    else
    {
      t << "<a class=\"el\" ";
    }
    t << "href=\"";
    if (ref)
    {
      if ((dest=tagDestinationDict[ref])) t << *dest << "/";
    }
    if (f) t << f << ".html\">";
  }
  else
  {
    t << "<b>";
  }
  docify(name);
  if (ref || f)
  {
    t << "</a>" << endl;
  }
  else
  {
    t << "</b>";
  }
  //if (Config::htmlHelpFlag && f)
  //{
  //  htmlHelp->addItem(name,((QCString)f)+".html");
  //}
}

void HtmlGenerator::writeStartAnnoItem(const char *,const char *f,
                                       const char *path,const char *name)
{
  t << "<li>";
  if (path) docify(path);
  t << "<a class=\"el\" href=\"" << f << ".html\">";
  docify(name);
  t << "</a> ";
  //if (Config::htmlHelpFlag && f)
  //{
  //  htmlHelp->addItem(name, ((QCString)f)+".html");
  //}
}

void HtmlGenerator::writeObjectLink(const char *ref,const char *f,
                                    const char *anchor, const char *name)
{
  QCString *dest;
  if (ref) 
  {
    t << "<a class=\"elRef\" ";
    t << "doxygen=\"" << ref << ":";
    if ((dest=tagDestinationDict[ref])) t << *dest << "/";
    t << "\" ";
  }
  else
  {
    t << "<a class=\"el\" ";
  }
  t << "href=\"";
  if (ref)
  {
    if ((dest=tagDestinationDict[ref])) t << *dest << "/";
  }
  if (f) t << f << ".html";
  if (anchor) t << "#" << anchor;
  t << "\">";
  docify(name);
  t << "</a>";
}

void HtmlGenerator::writeCodeLink(const char *ref,const char *f,
                                  const char *anchor, const char *name)
{
  QCString *dest;
  if (ref) 
  {
    t << "<a class=\"codeRef\" ";
    t << "doxygen=\"" << ref << ":";
    if ((dest=tagDestinationDict[ref])) t << *dest << "/";
    t << "\" ";
  }
  else
  {
    t << "<a class=\"code\" ";
  }
  t << "href=\"";
  if (ref)
  {
    if ((dest=tagDestinationDict[ref])) t << *dest << "/";
  }
  if (f) t << f << ".html";
  if (anchor) t << "#" << anchor;
  t << "\">";
  docify(name);
  t << "</a>";
  col+=strlen(name);
}

void HtmlGenerator::startTextLink(const char *f,const char *anchor)
{
  t << "<a href=\"";
  if (f)   t << f << ".html";
  if (anchor) t << "#" << anchor;
  t << "\">"; 
}

void HtmlGenerator::endTextLink()
{
  t << "</a>";
}

void HtmlGenerator::writeHtmlLink(const char *url,const char *text)
{
  t << "<a href=\"";
  if (url) t << url;
  t << "\">"; 
  docify(text);
  t << "</a>";
}

void HtmlGenerator::writeMailLink(const char *url)
{
  t << "<a href=\"mailto:" << url << "\">";
  docify(url); 
  t << "</a>";
}

void HtmlGenerator::startGroupHeader()
{
  t << "<h2>";
}

void HtmlGenerator::endGroupHeader()
{
  t << "</h2>" << endl;
}

void HtmlGenerator::writeSection(const char *lab,const char *title,bool sub)
{
  t << "<a name=\"" << lab << "\">";
  if (sub) t << "<h3>"; else t << "<h2>";
  docify(title);
  if (sub) t << "</h3>"; else t << "</h2>";
  t << "</a>" << endl;
}

void HtmlGenerator::writeSectionRef(const char *name,const char *lab,
                                    const char *title)
{
  QCString refName=name;
  if (refName.right(5)!=".html") refName+=".html";
  t << "<a href=\"" << refName << "#" << lab << "\">";
  docify(title);
  t << "</a>";
}

void HtmlGenerator::writeSectionRefItem(const char *name,const char *lab,
                                    const char *title)
{
  QCString refName=name;
  if (refName.right(5)!=".html") refName+=".html";
  t << "<a href=\"" << refName << "#" << lab << "\">";
  docify(title);
  t << "</a>";
}

void HtmlGenerator::writeSectionRefAnchor(const char *name,const char *lab,
                                          const char *title)
{
  writeSectionRef(name,lab,title);
}

void HtmlGenerator::docify(const char *str)
{
  if (str)
  {
    const char *p=str;
    char c;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '<':  t << "&lt;"; break;
        case '>':  t << "&gt;"; break;
        case '&':  t << "&amp;"; break;
        case '\\':
                   if (*p=='<')
                     { t << "&lt;"; p++; }
                   else if (*p=='>')
                     { t << "&gt;"; p++; }
                   else
                     t << "\\";
                   break;
        default:   t << c; 
      }
    }
  }
}

void HtmlGenerator::codify(const char *str)
{
  //docify(str);
  //static char spaces[]="        ";
  if (str)
  { 
    const char *p=str;
    char c;
    int spacesToNextTabStop;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '\t': spacesToNextTabStop = 
                         Config::tabSize - (col%Config::tabSize); 
                   t << spaces.left(spacesToNextTabStop); 
                   col+=spacesToNextTabStop; 
                   break; 
        case '\n': t << '\n'; col=0; 
                   break;
        case '<':  t << "&lt;"; col++; 
                   break;
        case '>':  t << "&gt;"; col++; 
                   break;
        case '&':  t << "&amp;"; col++; 
                   break;
        case '\\':
                   if (*p=='<')
                     { t << "&lt;"; p++; }
                   else if (*p=='>')
                     { t << "&gt;"; p++; }
                   else
                     t << "\\";
                   col++;
                   break;
        default:   t << c;    
                   col++;                    
                   break;
      }
    }
  }
}

void HtmlGenerator::writeChar(char c)
{
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}

void HtmlGenerator::startClassDiagram()
{
  t << "<p>";
}

void HtmlGenerator::endClassDiagram(ClassDiagram &d,
                                const char *fileName,const char *name)
{
  t << ":\n<p><center><img src=\""
    << fileName << ".gif\" usemap=\"#" << name << "_map\""
    << " border=\"0\"></center>" << endl
    << "<map name=\"" << name << "_map\"><p>" << endl;

  d.writeImageMap(t,dir,fileName);
}

void HtmlGenerator::startColorFont(uchar red,uchar green,uchar blue)
{
  QCString colorString;
  colorString.sprintf("%02x%02x%02x",red,green,blue);
  t << "<font color=\"#" << colorString << "\">";
}

void HtmlGenerator::endColorFont()
{
  t << "</font>";
}

void HtmlGenerator::writeFormula(const char *n,const char *text)
{
  if (text && text[0]=='\\') t << "<p><center>" << endl;
  t << "<img align=\"top\" src=\"" << n << "\">" << endl;
  if (text && text[0]=='\\') t << "</center><p>" << endl;
}

void HtmlGenerator::startMemberList()  
{ 
  DBG_HTML(t << "<!-- startMemberList -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
  }
  else
  {
    t << "<ul>" << endl; 
  }
}

void HtmlGenerator::endMemberList()    
{ 
  DBG_HTML(t << "<!-- endMemberList -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
  }
  else
  {
    t << "</ul>" << endl; 
  }
}

// annonymous type:
//  0 = single column right aligned
//  1 = double column left aligned
//  2 = single column left aligned
void HtmlGenerator::startMemberItem(int annoType) 
{ 
  DBG_HTML(t << "<!-- startMemberItem() -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "<tr>";
    switch(annoType)
    {
      case 0:  t << "<td nowrap align=right valign=top>"; break;
      case 1:  t << "<td nowrap>"; break;
      default: t << "<td nowrap valign=top>"; break;
    }
  }
  else
  {
    t << "<li>"; 
  }
}

void HtmlGenerator::endMemberItem(bool) 
{ 
  //DBG_HTML(t << "<!-- endMemberItem(" << (int)inGroup << "," << fileName << "," << headerName << " -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "</td></tr>"; 
  }
  t << endl; 
}

void HtmlGenerator::startParameter(bool first)
{
  if (first)
  {
    t << endl << "</b></td>" << endl;
    t << "<td><b>" << endl;
  }
  else
  {
    t << "<tr><td></td><td><b>" << endl;
  }
}

void HtmlGenerator::endParameter(bool)
{
  t << "</b></td></tr>" << endl;
}


void HtmlGenerator::insertMemberAlign() 
{ 
  DBG_HTML(t << "<!-- insertMemberAlign -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "&nbsp;</td><td valign=top>"; 
  }
}

void HtmlGenerator::startMemberDescription() 
{ 
  DBG_HTML(t << "<!-- startMemberDescription -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    //t << "<tr><td><img src=null.gif></td><td><img src=null.gif></td>"
    //     "<td></td><td><font size=-1><em>"; 
    t << "<tr><td>&nbsp;</td><td><font size=-1><em>"; 
  }
  else
  {
    t << "<dl class=\"el\"><dd><font size=-1><em>";
  }
}

void HtmlGenerator::endMemberDescription()   
{ 
  DBG_HTML(t << "<!-- endMemberDescription -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "</em></font><br><br></td></tr>" << endl; 
  }
  else
  {
    t << "</em></font></dl>";
  }
}

void HtmlGenerator::startMemberSections()
{
  DBG_HTML(t << "<!-- startMemberSections -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "<table border=0 cellpadding=0 cellspacing=0>" << endl;
  }
}

void HtmlGenerator::endMemberSections()
{
  DBG_HTML(t << "<!-- endMemberSections -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "</table>" << endl;
  }
}

void HtmlGenerator::startMemberHeader()
{
  DBG_HTML(t << "<!-- startMemberHeader -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "<tr><td colspan=2><br><h2>";
  }
  else
  {
    startGroupHeader();
  }
}

void HtmlGenerator::endMemberHeader()
{
  DBG_HTML(t << "<!-- endMemberHeader -->" << endl)
  if (Config::htmlAlignMemberFlag)
  {
    t << "</h2></td></tr>" << endl;
  }
  else
  {
    endGroupHeader();
  }
}

void HtmlGenerator::startMemberSubtitle()
{
  DBG_HTML(t << "<!-- startMemberSubtitle -->" << endl)
  if (Config::htmlAlignMemberFlag) t << "<tr><td colspan=2>";
}

void HtmlGenerator::endMemberSubtitle()
{
  DBG_HTML(t << "<!-- endMemberSubtitle -->" << endl)
  if (Config::htmlAlignMemberFlag) t << "<br><br></td></tr>" << endl;
}

void HtmlGenerator::startIndexList() 
{ 
  t << "<ul>"  << endl; 
  //if (Config::htmlHelpFlag)
  //{
  //  if (htmlHelp->depth()==0) htmlHelp->addItem(lastTitle,lastFile);
  //  htmlHelp->incDepth();
  //}
}

void HtmlGenerator::endIndexList()
{
  t << "</ul>" << endl;
  //if (Config::htmlHelpFlag)
  //{
  //  htmlHelp->decDepth();
  //}
}

void HtmlGenerator::startAlphabeticalIndexList()
{
  t << "<table align=center width=\"95%\" border=0 cellspacing=0 cellpadding=0>" << endl;
}

void HtmlGenerator::endAlphabeticalIndexList()
{
  t << "</table>" << endl;
}

void HtmlGenerator::writeIndexHeading(const char *s)
{
  //t << "<dt><b><big>" << s << "</big></b><dd>" << endl;
  t << "<div class=\"ah\"><font color=\"white\"><b>&nbsp;&nbsp;" << s 
    << "&nbsp;&nbsp;</b></font></div>";
}

void HtmlGenerator::writeImage(const char *name,const char *,const char *)
{
  QCString baseName=name;
  int i;
  if ((i=baseName.findRev('/'))!=-1 || (i=baseName.findRev('\\'))!=-1)
  {
    baseName=baseName.right(baseName.length()-i-1); 
  }
  t << "<img src=\"" << name << "\" alt=\"" << baseName << "\">" << endl;
}

void HtmlGenerator::startMemberDoc(const char *,const char *,const char *,const char *) 
{ 
  t << "<p>" << endl;
  t << "<table width=100% cellpadding=2 cellspacing=0 border=0>" << endl;
  t << "  <tr>" << endl;
  t << "    <td class=\"md\"><b>" << endl; 
  t << "      <table cellspadding=0 cellspacing=0 border=0>" << endl;
  t << "        <tr>" << endl;
  t << "          <td><b>" << endl;
}

void HtmlGenerator::endMemberDoc()     
{ 
  t << endl;
  t << "          </b></td>" << endl;
  t << "        </tr>" << endl;
  t << "      </table>" << endl;
  t << "    </td>" << endl;
  t << "  </tr>" << endl;
  t << "</table>" << endl; 
}

void HtmlGenerator::startCollaborationDiagram()
{
}

void HtmlGenerator::endCollaborationDiagram(DotGfxUsageGraph &g)
{
  g.writeGraph(t,Config::htmlOutputDir);
}

void HtmlGenerator::startInclDepGraph()
{
}

void HtmlGenerator::endInclDepGraph(DotInclDepGraph &g)
{
  g.writeGraph(t,Config::htmlOutputDir);
}

void HtmlGenerator::writeGraphicalHierarchy(DotGfxHierarchyTable &g)
{
  g.writeGraph(t,Config::htmlOutputDir);
}

void HtmlGenerator::startMemberGroupHeader()
{
  //t << "<tr><td colspan=2><img src=\"null.gif\" height=8></td></tr>" << endl;
  //t << "<tr><td colspan=2>" << endl;
  //t << "  <div class=\"in\">" << endl;
  //t << "    <table border=0 cellspacing=0 cellpadding=0>" << endl;
  //t << "      <tr><td colspan=3 class=\"groupLine\"><img src=\"null.gif\" height=1></td></tr>" << endl;
  //t << "      <tr>" << endl;
  //t << "        <td class=\"groupLine\"><img src=\"null.gif\" width=1></td>" << endl;
  //t << "        <td><div class=\"groupHeader\">&nbsp;&nbsp;";

  t << "<tr><td colspan=2><div class=\"groupHeader\">";
}

void HtmlGenerator::endMemberGroupHeader()
{
  //t << "&nbsp;&nbsp;</div></td>" << endl;
  //t << "        <td class=\"groupLine\"><img src=\"null.gif\" width=1></td>" << endl;
  //t << "      </tr>" << endl;
  //t << "      <tr><td colspan=3 class=\"groupLine\"><img src=\"null.gif\" height=1></td></tr>" << endl;
  //t << "    </table>" << endl;
  //t << "  </div>" << endl;
  //t << "</tr>" << endl;
  //t << "<tr><td colspan=2><img src=\"null.gif\" height=4></td></tr>" << endl;

  t << "</div></td></tr>" << endl;
}

void HtmlGenerator::startMemberGroupDocs()
{
  //t << "<tr><td colspan=2><img src=\"null.gif\" height=4></td></tr>" << endl;
  t << "<tr><td colspan=2><div class=\"groupText\">";
}

void HtmlGenerator::endMemberGroupDocs()
{
  t << "<br><br></div></td></tr>" << endl;
}

void HtmlGenerator::startMemberGroup()
{
}

void HtmlGenerator::endMemberGroup(bool)
{
}

