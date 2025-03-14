import unreal

asset_path = "/Game/Blueprints/del"

if unreal.EditorAssetLibrary.does_asset_exist(asset_path):

    unreal.EditorAssetLibrary.delete_asset(asset_path)
    print(f"Successfully deleted {asset_path}")
else:
    print(f"Asset {asset_path} does not exist")