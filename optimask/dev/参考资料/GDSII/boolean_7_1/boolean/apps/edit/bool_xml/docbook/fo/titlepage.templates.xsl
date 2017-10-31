<?xml version="1.0" encoding="utf-8"?><xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<!-- This stylesheet was created by titlepage.xsl; do not edit it by hand. -->

<xsl:template name="article.titlepage.recto">
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="(articleinfo/title|artheader/title|title)[1]"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="(articleinfo/subtitle|artheader/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/corpauthor|artheader/corpauthor"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/authorgroup|artheader/authorgroup"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/author|artheader/author"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/releaseinfo|artheader/releaseinfo"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/copyright|artheader/copyright"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/legalnotice|artheader/legalnotice"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/pubdate|artheader/pubdate"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/revision|artheader/revision"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/revhistory|artheader/revhistory"/>
  <xsl:apply-templates mode="article.titlepage.recto.mode" select="articleinfo/abstract|artheader/abstract"/>
</xsl:template>

<xsl:template name="article.titlepage.verso">
</xsl:template>

<xsl:template name="article.titlepage.separator">
</xsl:template>

<xsl:template name="article.titlepage.before.recto">
</xsl:template>

<xsl:template name="article.titlepage.before.verso">
</xsl:template>

<xsl:template name="article.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="article.titlepage.before.recto"/>
    <xsl:call-template name="article.titlepage.recto"/>
    <xsl:call-template name="article.titlepage.before.verso"/>
    <xsl:call-template name="article.titlepage.verso"/>
    <xsl:call-template name="article.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="article.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="article.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="set.titlepage.recto">
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="(setinfo/title|title)[1]"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="(setinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/corpauthor"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/authorgroup"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/author"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/releaseinfo"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/copyright"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/legalnotice"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/pubdate"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/revision"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/revhistory"/>
  <xsl:apply-templates mode="set.titlepage.recto.mode" select="setinfo/abstract"/>
</xsl:template>

<xsl:template name="set.titlepage.verso">
</xsl:template>

<xsl:template name="set.titlepage.separator">
</xsl:template>

<xsl:template name="set.titlepage.before.recto">
</xsl:template>

<xsl:template name="set.titlepage.before.verso">
</xsl:template>

<xsl:template name="set.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="set.titlepage.before.recto"/>
    <xsl:call-template name="set.titlepage.recto"/>
    <xsl:call-template name="set.titlepage.before.verso"/>
    <xsl:call-template name="set.titlepage.verso"/>
    <xsl:call-template name="set.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="set.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="set.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="book.titlepage.recto">
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="(bookinfo/title|title)[1]"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="(bookinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/corpauthor"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/authorgroup"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/author"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/releaseinfo"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/copyright"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/legalnotice"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/pubdate"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/revision"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/revhistory"/>
  <xsl:apply-templates mode="book.titlepage.recto.mode" select="bookinfo/abstract"/>
</xsl:template>

<xsl:template name="book.titlepage.verso">
</xsl:template>

<xsl:template name="book.titlepage.separator">
</xsl:template>

<xsl:template name="book.titlepage.before.recto">
</xsl:template>

<xsl:template name="book.titlepage.before.verso">
</xsl:template>

<xsl:template name="book.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="book.titlepage.before.recto"/>
    <xsl:call-template name="book.titlepage.recto"/>
    <xsl:call-template name="book.titlepage.before.verso"/>
    <xsl:call-template name="book.titlepage.verso"/>
    <xsl:call-template name="book.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="book.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="book.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="part.titlepage.recto">
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="(partinfo/title|docinfo/title|title)[1]"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="(partinfo/subtitle|docinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/corpauthor|docinfo/corpauthor"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/authorgroup|docinfo/authorgroup"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/author|docinfo/author"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/releaseinfo|docinfo/releaseinfo"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/copyright|docinfo/copyright"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/legalnotice|docinfo/legalnotice"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/pubdate|docinfo/pubdate"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/revision|docinfo/revision"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/revhistory|docinfo/revhistory"/>
  <xsl:apply-templates mode="part.titlepage.recto.mode" select="partinfo/abstract|docinfo/abstract"/>
</xsl:template>

<xsl:template name="part.titlepage.verso">
</xsl:template>

<xsl:template name="part.titlepage.separator">
</xsl:template>

<xsl:template name="part.titlepage.before.recto">
</xsl:template>

<xsl:template name="part.titlepage.before.verso">
</xsl:template>

<xsl:template name="part.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="part.titlepage.before.recto"/>
    <xsl:call-template name="part.titlepage.recto"/>
    <xsl:call-template name="part.titlepage.before.verso"/>
    <xsl:call-template name="part.titlepage.verso"/>
    <xsl:call-template name="part.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="part.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="part.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="reference.titlepage.recto">
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="(referenceinfo/title|docinfo/title|title)[1]"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="(referenceinfo/subtitle|docinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/corpauthor|docinfo/corpauthor"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/authorgroup|docinfo/authorgroup"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/author|docinfo/author"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/releaseinfo|docinfo/releaseinfo"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/copyright|docinfo/copyright"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/legalnotice|docinfo/legalnotice"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/pubdate|docinfo/pubdate"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/revision|docinfo/revision"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/revhistory|docinfo/revhistory"/>
  <xsl:apply-templates mode="reference.titlepage.recto.mode" select="referenceinfo/abstract|docinfo/abstract"/>
</xsl:template>

<xsl:template name="reference.titlepage.verso">
</xsl:template>

<xsl:template name="reference.titlepage.separator">
</xsl:template>

<xsl:template name="reference.titlepage.before.recto">
</xsl:template>

<xsl:template name="reference.titlepage.before.verso">
</xsl:template>

<xsl:template name="reference.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="reference.titlepage.before.recto"/>
    <xsl:call-template name="reference.titlepage.recto"/>
    <xsl:call-template name="reference.titlepage.before.verso"/>
    <xsl:call-template name="reference.titlepage.verso"/>
    <xsl:call-template name="reference.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="reference.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="reference.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="preface.titlepage.recto">
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="(prefaceinfo/title|docinfo/title|title)[1]"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="(prefaceinfo/subtitle|docinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/corpauthor|docinfo/corpauthor"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/authorgroup|docinfo/authorgroup"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/author|docinfo/author"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/releaseinfo|docinfo/releaseinfo"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/copyright|docinfo/copyright"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/legalnotice|docinfo/legalnotice"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/pubdate|docinfo/pubdate"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/revision|docinfo/revision"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/revhistory|docinfo/revhistory"/>
  <xsl:apply-templates mode="preface.titlepage.recto.mode" select="prefaceinfo/abstract|docinfo/abstract"/>
</xsl:template>

<xsl:template name="preface.titlepage.verso">
</xsl:template>

<xsl:template name="preface.titlepage.separator">
</xsl:template>

<xsl:template name="preface.titlepage.before.recto">
</xsl:template>

<xsl:template name="preface.titlepage.before.verso">
</xsl:template>

<xsl:template name="preface.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="preface.titlepage.before.recto"/>
    <xsl:call-template name="preface.titlepage.recto"/>
    <xsl:call-template name="preface.titlepage.before.verso"/>
    <xsl:call-template name="preface.titlepage.verso"/>
    <xsl:call-template name="preface.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="preface.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="preface.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="chapter.titlepage.recto">
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="(chapterinfo/title|docinfo/title|title)[1]"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="(chapterinfo/subtitle|docinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/corpauthor|docinfo/corpauthor"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/authorgroup|docinfo/authorgroup"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/author|docinfo/author"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/releaseinfo|docinfo/releaseinfo"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/copyright|docinfo/copyright"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/legalnotice|docinfo/legalnotice"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/pubdate|docinfo/pubdate"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/revision|docinfo/revision"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/revhistory|docinfo/revhistory"/>
  <xsl:apply-templates mode="chapter.titlepage.recto.mode" select="chapterinfo/abstract|docinfo/abstract"/>
</xsl:template>

<xsl:template name="chapter.titlepage.verso">
</xsl:template>

<xsl:template name="chapter.titlepage.separator">
</xsl:template>

<xsl:template name="chapter.titlepage.before.recto">
</xsl:template>

<xsl:template name="chapter.titlepage.before.verso">
</xsl:template>

<xsl:template name="chapter.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="chapter.titlepage.before.recto"/>
    <xsl:call-template name="chapter.titlepage.recto"/>
    <xsl:call-template name="chapter.titlepage.before.verso"/>
    <xsl:call-template name="chapter.titlepage.verso"/>
    <xsl:call-template name="chapter.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="chapter.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="chapter.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="appendix.titlepage.recto">
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="(appendixinfo/title|docinfo/title|title)[1]"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="(appendixinfo/subtitle|docinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/corpauthor|docinfo/corpauthor"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/authorgroup|docinfo/authorgroup"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/author|docinfo/author"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/releaseinfo|docinfo/releaseinfo"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/copyright|docinfo/copyright"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/legalnotice|docinfo/legalnotice"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/pubdate|docinfo/pubdate"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/revision|docinfo/revision"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/revhistory|docinfo/revhistory"/>
  <xsl:apply-templates mode="appendix.titlepage.recto.mode" select="appendixinfo/abstract|docinfo/abstract"/>
</xsl:template>

<xsl:template name="appendix.titlepage.verso">
</xsl:template>

<xsl:template name="appendix.titlepage.separator">
</xsl:template>

<xsl:template name="appendix.titlepage.before.recto">
</xsl:template>

<xsl:template name="appendix.titlepage.before.verso">
</xsl:template>

<xsl:template name="appendix.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="appendix.titlepage.before.recto"/>
    <xsl:call-template name="appendix.titlepage.recto"/>
    <xsl:call-template name="appendix.titlepage.before.verso"/>
    <xsl:call-template name="appendix.titlepage.verso"/>
    <xsl:call-template name="appendix.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="appendix.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="appendix.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="section.titlepage.recto">
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="(sectioninfo/title|title)[1]"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="(sectioninfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/corpauthor"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/authorgroup"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/author"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/releaseinfo"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/copyright"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/legalnotice"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/pubdate"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/revision"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/revhistory"/>
  <xsl:apply-templates mode="section.titlepage.recto.mode" select="sectioninfo/abstract"/>
</xsl:template>

<xsl:template name="section.titlepage.verso">
</xsl:template>

<xsl:template name="section.titlepage.separator">
</xsl:template>

<xsl:template name="section.titlepage.before.recto">
</xsl:template>

<xsl:template name="section.titlepage.before.verso">
</xsl:template>

<xsl:template name="section.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="section.titlepage.before.recto"/>
    <xsl:call-template name="section.titlepage.recto"/>
    <xsl:call-template name="section.titlepage.before.verso"/>
    <xsl:call-template name="section.titlepage.verso"/>
    <xsl:call-template name="section.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="section.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="section.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="sect1.titlepage.recto">
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="(sect1info/title|title)[1]"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="(sect1info/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/corpauthor"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/authorgroup"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/author"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/releaseinfo"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/copyright"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/legalnotice"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/pubdate"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/revision"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/revhistory"/>
  <xsl:apply-templates mode="sect1.titlepage.recto.mode" select="sect1info/abstract"/>
</xsl:template>

<xsl:template name="sect1.titlepage.verso">
</xsl:template>

<xsl:template name="sect1.titlepage.separator">
</xsl:template>

<xsl:template name="sect1.titlepage.before.recto">
</xsl:template>

<xsl:template name="sect1.titlepage.before.verso">
</xsl:template>

<xsl:template name="sect1.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="sect1.titlepage.before.recto"/>
    <xsl:call-template name="sect1.titlepage.recto"/>
    <xsl:call-template name="sect1.titlepage.before.verso"/>
    <xsl:call-template name="sect1.titlepage.verso"/>
    <xsl:call-template name="sect1.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="sect1.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="sect1.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="sect2.titlepage.recto">
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="(sect2info/title|title)[1]"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="(sect2info/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/corpauthor"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/authorgroup"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/author"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/releaseinfo"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/copyright"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/legalnotice"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/pubdate"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/revision"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/revhistory"/>
  <xsl:apply-templates mode="sect2.titlepage.recto.mode" select="sect2info/abstract"/>
</xsl:template>

<xsl:template name="sect2.titlepage.verso">
</xsl:template>

<xsl:template name="sect2.titlepage.separator">
</xsl:template>

<xsl:template name="sect2.titlepage.before.recto">
</xsl:template>

<xsl:template name="sect2.titlepage.before.verso">
</xsl:template>

<xsl:template name="sect2.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="sect2.titlepage.before.recto"/>
    <xsl:call-template name="sect2.titlepage.recto"/>
    <xsl:call-template name="sect2.titlepage.before.verso"/>
    <xsl:call-template name="sect2.titlepage.verso"/>
    <xsl:call-template name="sect2.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="sect2.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="sect2.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="sect3.titlepage.recto">
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="(sect3info/title|title)[1]"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="(sect3info/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/corpauthor"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/authorgroup"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/author"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/releaseinfo"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/copyright"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/legalnotice"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/pubdate"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/revision"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/revhistory"/>
  <xsl:apply-templates mode="sect3.titlepage.recto.mode" select="sect3info/abstract"/>
</xsl:template>

<xsl:template name="sect3.titlepage.verso">
</xsl:template>

<xsl:template name="sect3.titlepage.separator">
</xsl:template>

<xsl:template name="sect3.titlepage.before.recto">
</xsl:template>

<xsl:template name="sect3.titlepage.before.verso">
</xsl:template>

<xsl:template name="sect3.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="sect3.titlepage.before.recto"/>
    <xsl:call-template name="sect3.titlepage.recto"/>
    <xsl:call-template name="sect3.titlepage.before.verso"/>
    <xsl:call-template name="sect3.titlepage.verso"/>
    <xsl:call-template name="sect3.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="sect3.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="sect3.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="sect4.titlepage.recto">
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="(sect4info/title|title)[1]"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="(sect4info/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/corpauthor"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/authorgroup"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/author"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/releaseinfo"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/copyright"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/legalnotice"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/pubdate"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/revision"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/revhistory"/>
  <xsl:apply-templates mode="sect4.titlepage.recto.mode" select="sect4info/abstract"/>
</xsl:template>

<xsl:template name="sect4.titlepage.verso">
</xsl:template>

<xsl:template name="sect4.titlepage.separator">
</xsl:template>

<xsl:template name="sect4.titlepage.before.recto">
</xsl:template>

<xsl:template name="sect4.titlepage.before.verso">
</xsl:template>

<xsl:template name="sect4.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="sect4.titlepage.before.recto"/>
    <xsl:call-template name="sect4.titlepage.recto"/>
    <xsl:call-template name="sect4.titlepage.before.verso"/>
    <xsl:call-template name="sect4.titlepage.verso"/>
    <xsl:call-template name="sect4.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="sect4.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="sect4.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="sect5.titlepage.recto">
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="(sect5info/title|title)[1]"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="(sect5info/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/corpauthor"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/authorgroup"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/author"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/releaseinfo"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/copyright"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/legalnotice"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/pubdate"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/revision"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/revhistory"/>
  <xsl:apply-templates mode="sect5.titlepage.recto.mode" select="sect5info/abstract"/>
</xsl:template>

<xsl:template name="sect5.titlepage.verso">
</xsl:template>

<xsl:template name="sect5.titlepage.separator">
</xsl:template>

<xsl:template name="sect5.titlepage.before.recto">
</xsl:template>

<xsl:template name="sect5.titlepage.before.verso">
</xsl:template>

<xsl:template name="sect5.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="sect5.titlepage.before.recto"/>
    <xsl:call-template name="sect5.titlepage.recto"/>
    <xsl:call-template name="sect5.titlepage.before.verso"/>
    <xsl:call-template name="sect5.titlepage.verso"/>
    <xsl:call-template name="sect5.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="sect5.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="sect5.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template name="simplesect.titlepage.recto">
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="(simplesectinfo/title|docinfo/title|title)[1]"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="(simplesectinfo/subtitle|docinfo/subtitle|subtitle)[1]"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/corpauthor|docinfo/corpauthor"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/authorgroup|docinfo/authorgroup"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/author|docinfo/author"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/releaseinfo|docinfo/releaseinfo"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/copyright|docinfo/copyright"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/legalnotice|docinfo/legalnotice"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/pubdate|docinfo/pubdate"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/revision|docinfo/revision"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/revhistory|docinfo/revhistory"/>
  <xsl:apply-templates mode="simplesect.titlepage.recto.mode" select="simplesectinfo/abstract|docinfo/abstract"/>
</xsl:template>

<xsl:template name="simplesect.titlepage.verso">
</xsl:template>

<xsl:template name="simplesect.titlepage.separator">
</xsl:template>

<xsl:template name="simplesect.titlepage.before.recto">
</xsl:template>

<xsl:template name="simplesect.titlepage.before.verso">
</xsl:template>

<xsl:template name="simplesect.titlepage">
  <fo:block xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:call-template name="simplesect.titlepage.before.recto"/>
    <xsl:call-template name="simplesect.titlepage.recto"/>
    <xsl:call-template name="simplesect.titlepage.before.verso"/>
    <xsl:call-template name="simplesect.titlepage.verso"/>
    <xsl:call-template name="simplesect.titlepage.separator"/>
  </fo:block>
</xsl:template>

<xsl:template match="*" mode="simplesect.titlepage.recto.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="*" mode="simplesect.titlepage.verso.mode">
  <!-- if an element isn't found in this mode, -->
  <!-- try the generic titlepage.mode -->
  <xsl:apply-templates select="." mode="titlepage.mode"/>
</xsl:template>

</xsl:stylesheet>