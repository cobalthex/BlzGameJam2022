// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Growth_T7/Growth_T7GameModeBase.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGrowth_T7GameModeBase() {}
// Cross Module References
	GROWTH_T7_API UClass* Z_Construct_UClass_AGrowth_T7GameModeBase_NoRegister();
	GROWTH_T7_API UClass* Z_Construct_UClass_AGrowth_T7GameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_Growth_T7();
// End Cross Module References
	void AGrowth_T7GameModeBase::StaticRegisterNativesAGrowth_T7GameModeBase()
	{
	}
	UClass* Z_Construct_UClass_AGrowth_T7GameModeBase_NoRegister()
	{
		return AGrowth_T7GameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_AGrowth_T7GameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_Growth_T7,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "Growth_T7GameModeBase.h" },
		{ "ModuleRelativePath", "Growth_T7GameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGrowth_T7GameModeBase>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::ClassParams = {
		&AGrowth_T7GameModeBase::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGrowth_T7GameModeBase()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGrowth_T7GameModeBase_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGrowth_T7GameModeBase, 3231589197);
	template<> GROWTH_T7_API UClass* StaticClass<AGrowth_T7GameModeBase>()
	{
		return AGrowth_T7GameModeBase::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGrowth_T7GameModeBase(Z_Construct_UClass_AGrowth_T7GameModeBase, &AGrowth_T7GameModeBase::StaticClass, TEXT("/Script/Growth_T7"), TEXT("AGrowth_T7GameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGrowth_T7GameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
