/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
#include "stdafx.h"

#ifndef _MSC_VER
#include <QApplication>

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>

#if NDEBUG
//#define SET_LIBRARY_PATHS
#endif

#include <string>
#endif

#include "gui_wallet_global.hpp"
#include "mainwindow.hpp"

#include <QTranslator>

#include <graphene/utilities/dirhelper.hpp>
#include <graphene/utilities/git_revision.hpp>

using std::string;

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);
   QCoreApplication::setApplicationName("DECENT Wallet");
   QCoreApplication::setApplicationVersion(QString::fromStdString(graphene::utilities::git_version()));

   boost::program_options::options_description app_options("DECENT Wallet");
   boost::program_options::options_description cfg_options("DECENT Wallet");
   boost::program_options::variables_map options;

   try
   {
      gui_wallet::Globals::setCommandLine(app_options, cfg_options);
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, app_options), options);
   }
   catch (const boost::program_options::error& e)
   {
     std::cerr << "Error parsing command line: " << e.what() << "\n";
     return EXIT_FAILURE;
   }

   if( options.count("help") )
   {
      std::cout << app_options << std::endl;
      return EXIT_SUCCESS;
   }
   else if( options.count("version") )
   {
      std::cout << "DECENT Wallet " << graphene::utilities::git_version() << std::endl;
      return EXIT_SUCCESS;
   }

   QFile styleFile(":/css/styles/white_green.css");
   if(styleFile.open(QIODevice::ReadOnly))
   {
      QTextStream textStream(&styleFile);
      QString styleSheet = textStream.readAll();
      styleFile.close();
      app.setStyleSheet(styleSheet);
   }


   QTranslator* translator = new QTranslator();
   if (translator->load("decent_en", ":/translations/languages")) {
      app.installTranslator(translator);
   }

   app.setFont(gui_wallet::MainFont());
   gui_wallet::MainWindow aMainWindow(options["wallet-file"].as<std::string>());

   try {
      gui_wallet::Globals::instance().startDaemons(gui_wallet::BlockChainStartType::Simple, aMainWindow.walletFile());
   } catch (const std::exception& ex) {
      QMessageBox* msgBox = new QMessageBox();
      msgBox->setWindowTitle("Error");
      msgBox->setText(QString::fromStdString(ex.what()));
      msgBox->exec();
      std::cout << ex.what();
      exit(1);
   }
   
   
#define SET_LIBRARY_PATHS 1
#ifdef SET_LIBRARY_PATHS
   auto pluginsDir = QDir(QCoreApplication::applicationDirPath());
   if (pluginsDir.dirName() == "MacOS") {
      pluginsDir.cdUp();
   }
   pluginsDir.cd("plugins");
   

   QCoreApplication::setLibraryPaths(QStringList(pluginsDir.absolutePath()));
   QStringList paths = QCoreApplication::libraryPaths();

#endif

   try {

      qRegisterMetaType<string>( "std::string" );
      qRegisterMetaType<int64_t>( "int64_t" );
      app.setApplicationDisplayName("DECENT");


      aMainWindow.show();
      //aMainWindow.StartUpdateThread();
      app.exec();
   }
   catch(const std::exception& ex) {
      std::cout << "exception:" << ex.what() << std::endl;
   }
   catch(const fc::exception& ex) {
      std::cout << "exception:" << ex.what() << std::endl;
   }
   catch(...) {
      std::cout << "yay! exception..." << std::endl;
   }

   gui_wallet::Globals::instance().stopDaemons();
   gui_wallet::Globals::instance().clear();

   return 0;
}




